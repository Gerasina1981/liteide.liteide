package user32

import "syscall"
import "unsafe"

var (
	user32, _          = syscall.LoadLibrary("user32")
	procMessageBoxW, _ = syscall.GetProcAddress(user32, "MessageBoxW")
)

func MessageBoxW(handle uint32, text string, title string) {
	syscall.Syscall6(uintptr(procMessageBoxW), uintptr(handle), uintptr(unsafe.Pointer(syscall.StringToUTF16Ptr(text))), uintptr(unsafe.Pointer(syscall.StringToUTF16Ptr(title))), 0, 0, 0)
	return
}
