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
	{ 0xd8636a96, "unregister_framebuffer" },
	{ 0xf3a56073, "gpiod_get_value_cansleep" },
	{ 0x7ef76fee, "gpiod_set_value_cansleep" },
	{ 0xc3055d20, "usleep_range_state" },
	{ 0x78cbb017, "framebuffer_alloc" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xab1758f1, "fb_sys_write" },
	{ 0x122c3a7e, "_printk" },
	{ 0x2ec6e2fc, "sys_imageblit" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xf801920b, "sys_fillrect" },
	{ 0x40a9b349, "vzalloc" },
	{ 0x41c99599, "_dev_info" },
	{ 0x98b71c6, "fb_dealloc_cmap" },
	{ 0x71edef25, "spi_sync" },
	{ 0x32dab2e9, "register_framebuffer" },
	{ 0xe8eafa86, "driver_unregister" },
	{ 0x7a890c8, "fb_alloc_cmap" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xf2fa6626, "sys_copyarea" },
	{ 0x999e8297, "vfree" },
	{ 0x4081b721, "__spi_register_driver" },
	{ 0x7e138ecf, "devm_gpiod_get" },
	{ 0x931a2a3f, "fb_sys_read" },
	{ 0x9cdf6dde, "dev_err_probe" },
	{ 0xb5d1e30e, "framebuffer_release" },
	{ 0xf9a482f9, "msleep" },
	{ 0x8d5e53af, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cwaveshare,epd");
MODULE_ALIAS("of:N*T*Cwaveshare,epdC*");

MODULE_INFO(srcversion, "050C3854E73A4D59CFFA5A2");
