# 同济大学 操作系统暑期课程设计 (xv6 Labs 2025)

本仓库用于存放**同济大学操作系统暑期课程设计**（xv6-labs-2025）的实验代码。

为了保持主分支架构简洁清晰，`main` 分支仅作为项目说明文档，**各个实验的详细实现代码均存放在对应的独立分支中**。

---

##  实验分支索引



| 实验编号 | 实验名称 (Lab Name) | 对应分支 (Branch) | 核心实现内容概述 |
| :---: | :--- | :--- | :--- |
| **Lab 1** | **Xv6 and Unix utilities** | [`util`](../../tree/util) | 实现 `sleep`, `pingpong`, `primes`, `find`, `xargs` 等用户态实用工具 |
| **Lab 2** | **System Calls** | [`syscall`](../../tree/syscall) | 新增系统调用追踪（trace）与内存状态统计（sysinfo） |
| **Lab 3** | **Page Tables** | [`pgtbl`](../../tree/pgtbl) | 独立页表改造、`vmprint` 调试打印与用户态访问位检查 |
| **Lab 4** | **Traps** | [`traps`](../../tree/traps) | 栈帧分析、Backtrace 打印与周期性时钟中断回调（Alarm） |
| **Lab 5** | **Copy-on-Write** | [`cow`](../../tree/cow) | 实现写时复制（COW）页表机制与引用计数管理 |
| **Lab 6** | **Network Driver** | [`net`](../../tree/net) | 实现 E1000 网卡驱动环形缓冲区收发机制（E1000 tx/rx） |
| **Lab 7** | **Lock** | [`lock`](../../tree/lock) | 优化内存分配器细粒度锁（kalloc）与磁盘块缓存（bcache）并发锁 |
| **Lab 8** | **File System** | [`mmap`](../../tree/mmap) | 实现内存映射文件系统 `mmap` 及 `munmap` 系统调用 |
| **Lab 9** | **mmap** | [`mmap`](../../tree/mmap) | 实现内存映射文件系统 `mmap` 及 `munmap` 系统调用 |

---

