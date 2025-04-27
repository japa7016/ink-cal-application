#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x976e7127, "unregister_framebuffer" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x2f21e8f7, "framebuffer_alloc" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x7a48185f, "fb_sys_write" },
	{ 0x122c3a7e, "_printk" },
	{ 0xa81698ac, "sys_imageblit" },
	{ 0xee0a3a1f, "sys_fillrect" },
	{ 0x40a9b349, "vzalloc" },
	{ 0x1289fbda, "cdev_add" },
	{ 0xc9d8fecd, "register_framebuffer" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc08d969f, "sys_copyarea" },
	{ 0x999e8297, "vfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x4fde7176, "fb_sys_read" },
	{ 0x3a876d73, "framebuffer_release" },
	{ 0xd8287c5f, "cdev_init" },
	{ 0x7c127ea2, "cdev_del" },
	{ 0x2273f01b, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D541A9371FA0BFC233DBA3E");
