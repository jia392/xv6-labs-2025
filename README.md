# 同济大学 操作系统暑期课程设计 (xv6 Labs 2025)

本仓库用于存放**同济大学操作系统暑期课程设计**（xv6-labs-2025）的实验代码。

为了保持主分支架构简洁清晰，`main` 分支仅作为项目说明文档，**各个实验的详细实现代码均存放在对应的独立分支中**。

---

##  实验分支索引



| 编号 | 实验名称 (Lab Name) | 对应分支 (Branch) | 核心实现内容 |
| :---: | :--- | :--- | :--- |
| **Lab 1** | **Utilities** | [`util`](../../tree/util) | 实现 `sleep`, `sixfive`, `memdump`, `find`, `exec` 等用户态实用工具 |
| **Lab 2** | **System Calls** | [`syscall-lab`](../../tree/syscall) | 新增沙箱与安全机制，掌握内核调用 |
| **Lab 3** | **Page Tables** | [`pagetable`](../../tree/pgtbl) | 优化虚拟内存映射，支持 2MB 大页 |
| **Lab 4** | **Traps** | [`trap`](../../tree/traps) | 掌握中断与陷入机制，实现用户态信号回调及内核栈回溯 |
| **Lab 5** | **Copy-on-Write** | [`cow`](../../tree/cow) | 实现写时复制（COW），延迟物理页分配以优化 fork 性能 |
| **Lab 6** | **Network Driver** | [`net`](../../tree/net) | 编写 E1000 网卡驱动，并补充 UDP/IP 协议栈接收逻辑 |
| **Lab 7** | **Lock** | [`lock`](../../tree/lock) | 优化内存分配器与块缓存，设计读写锁 |
| **Lab 8** | **File System** | [`fs`](../../tree/mmap) | 扩展文件系统，支持双重间接索引大文件与软链接 |
| **Lab 9** | **mmap** | [`mmap`](../../tree/mmap) | 结合缺页中断与 VMA，实现文件内存映射 mmap/munmap |

---

##  环境依赖与编译

- **系统环境**: Ubuntu 24.04 (WSL2 / Linux)
- **工具链**: `gcc-riscv64-linux-gnu`, `qemu-system-riscv64`, `gdb-riscv64-linux-gnu`

### 运行与测试方法

1. **切换到目标实验分支**（以切换 Lab 7 为例）：
   ```bash
   git checkout lock

2. **编译并启动 xv6 模拟器**
   ```bash
   make qemu

4. **在 xv6 内部运行测试，例如：**
   ```bash
   kalloctest
   
6. **运行评分脚本进行本地测试**
    ```bash
   make grade
