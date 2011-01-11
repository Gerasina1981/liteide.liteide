LiteIDE

go语言开发工具

*) UTF8文件读写
*) GO语法高亮
*) 项目管理
*) 支持.pro项目编译
*) 支持无项目单独文件编译
*) gopromake自动编译工具
*) 可以从编译错误中定位到源码

开发者:    visualfc (visualfc@gamil.com)
项目主页: http://code.google.com/p/liteide

需要设置的go编译环境变量：
set path=%path%;c:\go\bin

FAQ:

１.如何进行项目管理
　在LiteIDE开发环境内打开项目，在LiteIDE环境中.pro文件直接进行编写，保盘后LiteIDE将自动更新项目，并重建项目文件列表。

2.项目文件编写举例
TARGET=hello
DESTDIR=./bin
GOFILES = \
	hello.go \
	user32.go


３.打开项目或文件
　可以从文件菜单中选择打开项目/打开文件，也可以直接将.pro或.go文件拖到LiteIDE界面上。

４.编译和运行
　如果存在项目，则编译和运行当前项目。
　如果没有项目，则编译和运行当前编辑文件。

