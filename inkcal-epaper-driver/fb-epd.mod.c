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
	{ 0x3d376961, "gpiod_get_value_cansleep" },
	{ 0xf0d7e4ae, "gpiod_set_value_cansleep" },
	{ 0xc3055d20, "usleep_range_state" },
	{ 0x2f21e8f7, "framebuffer_alloc" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x7a48185f, "fb_sys_write" },
	{ 0x122c3a7e, "_printk" },
	{ 0xa81698ac, "sys_imageblit" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xee0a3a1f, "sys_fillrect" },
	{ 0x40a9b349, "vzalloc" },
	{ 0xe456497a, "_dev_info" },
	{ 0x98b71c6, "fb_dealloc_cmap" },
	{ 0xc74b5d97, "spi_sync" },
	{ 0xc9d8fecd, "register_framebuffer" },
	{ 0x2c74bf9a, "driver_unregister" },
	{ 0x7a890c8, "fb_alloc_cmap" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc08d969f, "sys_copyarea" },
	{ 0x999e8297, "vfree" },
	{ 0xd0ee0882, "__spi_register_driver" },
	{ 0x4708e2f1, "devm_gpiod_get" },
	{ 0x4fde7176, "fb_sys_read" },
	{ 0x9fc81d9, "dev_err_probe" },
	{ 0x3a876d73, "framebuffer_release" },
	{ 0xf9a482f9, "msleep" },
	{ 0x2273f01b, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cwaveshare,epd");
MODULE_ALIAS("of:N*T*Cwaveshare,epdC*");

MODULE_INFO(srcversion, "7E656F9526C820A444E7672");
