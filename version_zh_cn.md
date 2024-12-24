build1　建立boot  
build2　建立Makefile  
build3　建立loader  
build4　加载loader  
build5　建立并加载kernel  
build6　进入保护模式并启用分页  
build7　进入kernel  
build8　建立C语言下的框架  
build9　调节文件结构  
build10　为中断提供默认处理程序  
build11　添加第一个进程  
build12　添加时钟中断（中断0x20）  
build13　根据时钟进行进程切换  
build14　完善中断接口  
build15　添加第一个系统调用（中断0x30）  
build16　使用时钟进行延时  
build17　实现进程优先级  
　　　　 取消boot和loader中的部分打印字符  
build18　处理键盘输入（英文字符）  
build19　处理键盘输入（shift之类特殊按键）  
build20　实现多终端  
build21　处理键盘输入（锁定键、小键盘等）  
build22　区分进程权限级（目前分为三级：kernel、system、user）  
build23　修复bug：process中ticks类型错误导致每个进程占用时间意外的长  
　　　　 优化代码  
build24　实现printf  
　　　　 修复bug：向控制台打印字符时会跳转到此控制台