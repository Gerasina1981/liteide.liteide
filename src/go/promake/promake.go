package main

import (
	"fmt"
	"os"
	"bytes"
	"regexp"
	"flag"
	"path"
	"exec"
	"syscall"
)

var gcFileName *string = flag.String("gc", "8g", "golang gc compiler 8g[6g][5g] filepath")
var glFileName *string = flag.String("gl", "8l", "golang linker 8l[6l][5l] filepath")
var arFileName *string = flag.String("ar", "gopack", "golang ar tool gopack filepath")
var cgoFileName *string = flag.String("cgo", "cgo", "golang call C cgo filepath")
var proFileName *string = flag.String("p", "", "build golang project file example.pro")
var goFileName *string = flag.String("f", "", "build golang source file example.go")
var printDep *bool = flag.Bool("dep", true, "print all files depends pakckage")

type Gopro struct {
	Name   string
	Values map[string][]string
	array *PackageArray
}

func makePro(name string) (pro *Gopro, err os.Error) {
	file, err := os.Open(name, os.O_RDONLY, 0)
	if err != nil {
		return
	}

	defer file.Close()

	pro = new(Gopro)
	pro.Name = name
	pro.Values = make(map[string][]string)

	var buf [512]byte
	n, err := file.Read(buf[:])

	pre, err := regexp.Compile("\\\\[^a-z|A-Z|0-9|_|=]*[\n\r]+[^a-z|A-Z|0-9|_]*")
	if err != nil {
		return
	}
	all := pre.ReplaceAll(buf[0:n], []byte(" "))

	lines := bytes.Split(all, []byte("\r\n"), -1)

	for _, line := range lines {
		offset := 2
		line = bytes.Replace(line, []byte("\t"), []byte(" "), -1)
		find := bytes.Index(line, []byte("+="))
		if find == -1 {
			offset = 1
			find = bytes.Index(line, []byte("="))
		}
		if find != -1 {
			k := bytes.TrimSpace(line[0:find])
			v := bytes.SplitAfter(line[find+offset:], []byte(" "), -1)
			var vall []string
			if offset == 2 {
				vall = pro.Values[string(k)]
			}
			for _, nv := range v {
				nv2 := bytes.TrimSpace(nv)
				if len(nv2) != 0 {
					vall = append(vall, string(nv2))
				}
			}
			pro.Values[string(k)] = vall
		}
	}
	return
}

func (file *Gopro) Gofiles() []string {
	return file.Values["GOFILES"]
}

func (file *Gopro) AllPackage() (paks []string) {
	for i := 0; i < len(file.array.Data); i++ {
		paks = append(paks,file.array.Data[i].pakname)
	}
	return
}

func (file *Gopro) PackageFilesString(pakname string) []string {
	return file.Values[pakname]
}

func (file *Gopro) PackageFiles(pakname string) []string {
	return file.array.index(pakname).files
}

func (file *Gopro) TargetName() string {
	v := file.Values["TARGET"]
	if len(v) >= 1 {
		return string(v[0])
	}
	ext := path.Ext(file.Name)
	name := path.Base(file.Name)
	return name[0 : len(name)-len(ext)]
}

func (file *Gopro) DestDir() (dir string) {
	v := file.Values["DESTDIR"]
	if len(v) >= 1 {
		dir = string(v[0])
	}
	return
}

func (file *Gopro) ProjectDir() (dir string) {
	dir, _ = path.Split(file.Name)
	return
}

func build(gcfile string, proFileName string, files []string, envv []string, dir string) (status syscall.WaitStatus, err os.Error) {
	arg := []string{gcfile, "-o", proFileName}
	for _, v := range files {
		arg = append(arg, string(v))
	}
	fmt.Println("\t", arg)
	var cmd *exec.Cmd
	cmd, err = exec.Run(gcfile, arg[:], envv[:], dir, 0, 1, 2)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	defer cmd.Close()
	var wait *os.Waitmsg
	wait, err = cmd.Wait(0)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	status = wait.WaitStatus
	return
}

func link(glfile string, target string, ofile string, envv []string, dir string) (status syscall.WaitStatus, err os.Error) {
	arg := []string{glfile, "-o", target, ofile}
	fmt.Println("\t", arg)
	var cmd *exec.Cmd
	cmd, err = exec.Run(glfile, arg[:], envv[:], dir, 0, 1, 2)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	defer cmd.Close()
	var wait *os.Waitmsg
	wait, err = cmd.Wait(0)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	status = wait.WaitStatus
	return
}

func pack(pkfile string, target string, ofile string, envv []string, dir string) (status syscall.WaitStatus, err os.Error) {
	arg := []string{pkfile, "grc", target, ofile}
	fmt.Println("\t", arg)
	var cmd *exec.Cmd
	cmd, err = exec.Run(pkfile, arg[:], envv[:], dir, 0, 1, 2)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	defer cmd.Close()
	var wait *os.Waitmsg
	wait, err = cmd.Wait(0)
	if err != nil {
		fmt.Printf("Error, %s", err)
		return
	}
	status = wait.WaitStatus
	return
}

func (file *Gopro) IsEmpty() bool {
	return len(file.Values) == 0
}

func MakeFile(gcfile, glfile string, file string) (status syscall.WaitStatus, err os.Error) {
	dir, name := path.Split(file)
	ext := path.Ext(file)
	title := name[0 : len(name)-len(ext)]

	ofile := title + ".8"
	files := []string{file}
	status, err = build(gcfile, ofile, files, os.Environ(), dir)
	if err != nil || status.ExitStatus() != 0 {
		return
	}
	target := title + ".exe"
	status, err = link(glfile, target, ofile, os.Environ(), dir)
	if err != nil || status.ExitStatus() != 0 {
		return
	}
	fmt.Printf("link target : %s\n", target)

	return
}


func (file *Gopro) MakeTarget(gcfile, glfile, pkfile string) (status syscall.WaitStatus, err os.Error) {
	all := file.AllPackage()
	for _, v := range all {
		fmt.Printf("build package %s:\n", v)
		target := v
		ofile := target + ".8"
		if v == "main" {
			target = file.TargetName()
			ofile = target + "_go_.8"
		}
		status, err = build(gcfile, ofile, file.PackageFiles(v), os.Environ(), file.ProjectDir())
		if err != nil || status.ExitStatus() != 0 {
			return
		}

		dest := file.DestDir()
		if len(dest) > 0 {
			dest = path.Join(file.ProjectDir(), dest)
			os.MkdirAll(dest, 0)
			target = path.Join(dest, target)
		}
		if string(v) == "main" {
			target = target + ".exe"
			status, err = link(glfile, target, ofile, os.Environ(), file.ProjectDir())
			if err != nil || status.ExitStatus() != 0 {
				return
			}
			fmt.Printf("link target : %s\n", target)
		} else {
			target = target + ".a"
			status, err = pack(pkfile, target, ofile, os.Environ(), file.ProjectDir())
			if err != nil || status.ExitStatus() != 0 {
				return
			}
			fmt.Printf("pack target : %s\n", target)
		}
	}
	return
}

func main() {
	fmt.Printf("Gopro : golan .pro file build tools. make by visualfc.\n\n")
	flag.Parse()
	gcfile := *gcFileName
	glfile := *glFileName
	pkfile := *arFileName
	if len(*proFileName) > 0 {
		file, _ := makePro(*proFileName)
		if file == nil {
			flag.Usage()
			return
		}

		if file.IsEmpty() {
			fmt.Printf("Error load .pro file %s\n", *proFileName)
			os.Exit(1)
		}
		files := file.Gofiles()
		file.array = ParserFiles(files)
		
		if (printDep != nil && *printDep == true) {
			fmt.Printf("AllPackage:\n%s\n",file.array)
		}

		status, err := file.MakeTarget(gcfile, glfile, pkfile)
		if err != nil || status.ExitStatus() != 0 {
			fmt.Printf("Error and exit!\n")
			os.Exit(1)
		}
	} else if len(*goFileName) > 0 {
		status, err := MakeFile(gcfile, glfile, *goFileName)
		if err != nil || status.ExitStatus() != 0 {
			fmt.Printf("Error and exit!\n")
			os.Exit(1)
		}
	} else {
		flag.Usage()
	}
	os.Exit(0)
}
