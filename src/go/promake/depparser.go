package main

import (
	"fmt"
	"os"
	"sort"
	"go/parser"
	"go/token"
	"go/ast"
)

func GetPackage(filename string) (pakname string, err os.Error) {
	var file *ast.File
	file, err = parser.ParseFile(token.NewFileSet(), filename, nil, parser.ImportsOnly)
	if err != nil {
		return
	}
	if file.Name != nil {
		pakname = file.Name.Name
	}
	return
}

func GetPackageImport(filename string) (pakname string, imports []string, err os.Error) {
	var file *ast.File
	file, err = parser.ParseFile(token.NewFileSet(), filename, nil, parser.ImportsOnly)
	if err != nil {
		return
	}
	if file.Name != nil {
		pakname = file.Name.Name
	}
	for _, d := range file.Decls {
		if gd, ok := d.(*ast.GenDecl); ok {
			if gd.Tok == token.IMPORT {
				for _, s := range gd.Specs {
					if v, ok := s.(*ast.ImportSpec); ok {
						imports = append(imports, string(v.Path.Value[1:len(v.Path.Value)-1]))
					}
				}
			}
		}
	}
	return
}

func GetPackageImportLocal(filename string) (pakname string, imports []string, err os.Error) {
	var file *ast.File
	file, err = parser.ParseFile(token.NewFileSet(), filename, nil, parser.ImportsOnly)
	if err != nil {
		return
	}
	if file.Name != nil {
		pakname = file.Name.Name
	}
	for _, d := range file.Decls {
		if gd, ok := d.(*ast.GenDecl); ok {
			if gd.Tok == token.IMPORT {
				for _, s := range gd.Specs {
					if v, ok := s.(*ast.ImportSpec); ok {
						if v.Path.Value[1] == '.' && v.Path.Value[2] == '/' {
							imports = append(imports, string(v.Path.Value[3:len(v.Path.Value)-1]))
						}
					}
				}
			}
		}
	}
	return
}

type PackageData struct {
	pakname string
	files   []string
	index   int
}

func (p *PackageData) appendFile(file string) {
	p.files = append(p.files, file)
}

type PackageArray struct {
	Data []*PackageData
	LocalImports map[string][]string
}

func (p *PackageArray) index(pakname string) (data *PackageData) {
	for i := 0; i < len(p.Data); i++ {
		if p.Data[i].pakname == pakname {
			data = p.Data[i]
			return
		}
	}
	data = new(PackageData)
	data.pakname = pakname
	p.Data = append(p.Data, data)
	return
}

func (p *PackageArray) Len() int           { return len(p.Data) }
func (p *PackageArray) Less(i, j int) bool { return p.Data[i].index < p.Data[j].index }
func (p *PackageArray) Swap(i, j int)      { p.Data[i], p.Data[j] = p.Data[j], p.Data[i] }
func (p *PackageArray) String() (info string) {
	for i := 0; i < len(p.Data); i++ {
		var data *PackageData = p.Data[i]
		info += fmt.Sprintln(data.pakname, data.files, data.index)
	}
	return
}

func ParserFiles(files []string) (array *PackageArray) {
	array = new(PackageArray)
	array.LocalImports = make(map[string][]string)

	for _, file := range files {
		pakname, imports, err := GetPackageImportLocal(file)
		if err == nil {
			array.index(pakname).appendFile(file)
			for _, v := range imports {
				array.LocalImports[pakname] = append(array.LocalImports[pakname], v)
			}
		}
	}
	for i := 0; i < len(array.Data); i++ {
		for _, f := range array.LocalImports[array.Data[i].pakname] {
			array.Data[i].index += array.index(f).index + 1
		}
	}

	sort.Sort(array)

	return
}