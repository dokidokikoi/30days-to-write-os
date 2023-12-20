void HariMain(void)
{

fin:
	/*这里虽然想写上HLT，但C语言中不能用HLT!*/
    io_hlt(); /*执行naskfunc.nas里的_io_hlt*/
	goto fin;

}