package main

import (
	"go/ast"
	"go/doc"
	"go/parser"
	"go/token"
	"os"
	"io"
	"fmt"
)

const (
	tag_package = "p"
	tag_type = "t"
	tag_struct = "s"
	tag_interface = "i"
	tag_value = "v"
	tag_const = "c"
	tag_value_folder = "+v:value"
	tag_const_folder = "+c:const"
	tag_func = "f"
	tag_func_folder = "+f:func:"
	tag_type_method = "m"
	tag_type_factor = "a"
)	

type PackageView struct {
	fset *token.FileSet
	pdoc *doc.PackageDoc
}

type AstPos struct{}

type ViewInterface interface{}

func NewFilePackage(filename string) (*PackageView, os.Error) {
	p := new(PackageView)
	p.fset = token.NewFileSet()
	file, err := parser.ParseFile(p.fset, filename, nil, 0)
	if err != nil {
		return nil, err
	}
	p.pdoc = doc.NewFileDoc(file)
	return p, nil
}

func (p *PackageView) PrintFuncs(w io.Writer, funcs []*doc.FuncDoc, level int, tag string, tag_folder string) {
	if len(tag_folder) > 0 && len(funcs) > 0{
		fmt.Fprintf(w,"%d:%s\n",level,tag_folder)
		level++
	}
			
	for _, f := range funcs {
		pos := p.fset.Position(f.Decl.Pos())
		fmt.Fprintf(w, "%d:%s:%s:%s\n", level,tag, f.Name, pos)
	}
}

func (p *PackageView) PrintVars(w io.Writer, vars []*doc.ValueDoc, level int,tag string, tag_folder string) {
	if len(tag_folder) > 0 && len(vars) > 0{
		fmt.Fprintf(w,"%d:%s\n",level,tag_folder)
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
					fmt.Fprintf(w, "%d:%s:%s:%s\n",level,tag, m.Names[i], pos)			
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
		fmt.Fprintf(w, "%d:%s:%s:%s\n", level,tag, d.Type.Name, pos)
		p.PrintFuncs(w, d.Factories, level+1,tag_type_factor,"")
		p.PrintFuncs(w, d.Methods, level+1,tag_type_method,"")
		p.PrintVars(w,d.Vars,level+1,tag_value,tag_value_folder)
	}
}

func (p *PackageView) PrintPackage(w io.Writer, level int) {
	fmt.Fprintf(w, "%d:%s:%s\n",level, tag_package,p.pdoc.PackageName)
	level++
	p.PrintFuncs(w, p.pdoc.Funcs, level, tag_func, "")
	p.PrintTypes(w, p.pdoc.Types, level)
	p.PrintVars(w, p.pdoc.Vars, level, tag_value,tag_value_folder)
	p.PrintVars(w,p.pdoc.Consts,level, tag_const,tag_const_folder)	
}

func (p *PackageView) PrintTree(w io.Writer) {
	p.PrintPackage(w,0)
}