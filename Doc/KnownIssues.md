* 当使用预编译`sfml`库 与`libc++` 时链接失败。这可能是因为预编译库直接使用了`libstdc++`，而`libc++` 与 `libstdc++` 并不完全兼容造成的。 
* 当绘制的图形较为复杂并处在Debug 模式下，有可能在绘制的时候出现 `EXC_BAD_ACCESS`(MacOS) 错误或者` Segmentation fault (core dumped)`(Linux)。这时需要使用更大的堆栈。