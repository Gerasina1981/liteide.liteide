package main

import (
	"go/ast"
	"go/parser"
	"go/token"
	"os"
	"io"
	"io/ioutil"
	"fmt"
	"./doc"
)

const (
	tag_package      = "p"
	tag_type         = "t"
	tag_struct       = "s"
	tag_interface    = "i"
	tag_value        = "v"
	tag_const        = "c"
	tag_value_folder = "+v"
	tag_const_folder = "+c"
	tag_func         = "f"
	tag_func_folder  = "+f"
	tag_type_method  = "m"
	tag_type_factor  = "a"
)

type PackageView struct {
	fset  *token.FileSet
	pdoc  *doc.PackageDoc
	files []string
}

func (p *PackageView) posFileIndex(pos token.Position) int {
	var index = -1
	for i := 0; i < len(p.files); i++ {
		if p.files[i] == pos.Filename {
			index = i
			break
		}
	}
	if index == -1 {
		p.files = append(p.files, pos.Filename)
		index = len(p.files) - 1
	}
	return index
}

func (p *PackageView) posText(pos token.Position) (s string) {
	index := p.posFileIndex(pos)
	return fmt.Sprintf("%d,%d,%d", index, pos.Line, pos.Column)
}

func NewFilePackage(filename string) (*PackageView, os.Error) {
	p := new(PackageView)
	p.fset = token.NewFileSet()
	p.files = append(p.files, filename)
	file, err := parser.ParseFile(p.fset, filename, nil, 0)
	if err != nil {
		return nil, err
	}
	p.pdoc = doc.NewFileDoc(file, true)
	return p, nil
}

func NewFilePackageSource(filename string, f *os.File) (*PackageView, os.Error) {
	src, err := ioutil.ReadAll(f)
	if err != nil {
		return nil, err
	}
	p := new(PackageView)
	p.fset = token.NewFileSet()
	p.files = append(p.files, filename)
	file, err := parser.ParseFile(p.fset, filename, src, 0)
	if err != nil {
		return nil, err
	}
	p.pdoc = doc.NewFileDoc(file, true)
	return p, nil
}


func (p *PackageView) PrintFuncs(w io.Writer, funcs []*doc.FuncDoc, level int, tag string, tag_folder string) {
	if len(tag_folder) > 0 && len(funcs) > 0 {
		fmt.Fprintf(w, "%d,%s,Functions\n", level, tag_folder)
		level++
	}

	for _, f := range funcs {
		pos := p.fset.Position(f.Decl.Pos())
		fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, f.Name, p.posText(pos))
	}
}

func (p *PackageView) PrintVars(w io.Writer, vars []*doc.ValueDoc, level int, tag string, tag_folder string) {
	if len(tag_folder) > 0 && len(vars) > 0 {
		if tag_folder == "+v" {
			fmt.Fprintf(w, "%d,%s,Variables\n", level, tag_folder)
		} else if tag_folder == "+c" {
			fmt.Fprintf(w, "%d,%s,Constants\n", level, tag_folder)
		}
		level++
	}
	for _, v := range vars {
		if v.Decl == nil {
			continue
		}
		for _, s := range v.Decl.Specs {
			if m, ok := s.(*ast.ValueSpec); ok {
				pos := p.fset.Position(m.Pos())
				for i := 0; i < len(m.Names); i++ {
					fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, m.Names[i], p.posText(pos))
				}
			}
		}
	}
}

func (p *PackageView) PrintTypes(w io.Writer, types []*doc.TypeDoc, level int) {
	for _, d := range types {
		if d.Type == nil {
			continue
		}
		var tag string = tag_type
		if _, ok := d.Type.Type.(*ast.InterfaceType); ok {
			tag = tag_interface
		} else if _, ok := d.Type.Type.(*ast.StructType); ok {
			tag = tag_struct
		}
		pos := p.fset.Position(d.Type.Pos())
		fmt.Fprintf(w, "%d,%s,%s,%s\n", level, tag, d.Type.Name, p.posText(pos))
		p.PrintFuncs(w, d.Factories, level+1, tag_type_factor, "")
		p.PrintFuncs(w, d.Methods, level+1, tag_type_method, "")
		p.PrintVars(w, d.Vars, level+1, tag_value, tag_value_folder)
	}
}

func (p *PackageView) PrintHeader(w io.Writer, level int) {
	fmt.Fprintf(w, "%d,%s,%s", level, tag_package, p.pdoc.PackageName)
	for i := 0; i < len(p.files); i++ {
		fmt.Fprintf(w, ",%s", p.files[i])
	}
	fmt.Fprintf(w, "\n")
}

func (p *PackageView) PrintPackage(w io.Writer, level int) {
	p.PrintHeader(w, level)
	level++
	p.PrintVars(w, p.pdoc.Vars, level, tag_value, tag_value_folder)
	p.PrintVars(w, p.pdoc.Consts, level, tag_const, tag_const_folder)
	p.PrintFuncs(w, p.pdoc.Funcs, level, tag_func, "")
	p.PrintTypes(w, p.pdoc.Types, level)
}
// level:tag:name:source:x:y
func (p *PackageView) PrintTree(w io.Writer) {
	p.PrintPackage(w, 0)
}
