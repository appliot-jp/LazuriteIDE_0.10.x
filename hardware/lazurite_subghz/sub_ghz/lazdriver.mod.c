#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xbbfdf386, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x3ce4ca6f, __VMLINUX_SYMBOL_STR(disable_irq) },
	{ 0x566d31c9, __VMLINUX_SYMBOL_STR(i2c_master_send) },
	{ 0x8e78d0f5, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x6c09c2a4, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0x364d6d72, __VMLINUX_SYMBOL_STR(i2c_del_driver) },
	{ 0xbcf89ab6, __VMLINUX_SYMBOL_STR(__wake_up_sync) },
	{ 0x39cc5bd, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xe1e9b159, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xd6feea4f, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x5464d80a, __VMLINUX_SYMBOL_STR(__spi_register_driver) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
	{ 0x9580deb, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0x9af40a52, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x79e08e1f, __VMLINUX_SYMBOL_STR(i2c_put_adapter) },
	{ 0xdb5bd0dd, __VMLINUX_SYMBOL_STR(spi_setup) },
	{ 0x9d29f46b, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0x275ef902, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x5f754e5a, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x509f7dc, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x12c190d6, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0xeec4978e, __VMLINUX_SYMBOL_STR(driver_unregister) },
	{ 0xa1c76e0a, __VMLINUX_SYMBOL_STR(_cond_resched) },
	{ 0x4d557e5a, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0xe183bd9, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0xefe6fe79, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x1bb31047, __VMLINUX_SYMBOL_STR(add_timer) },
	{ 0x39d78e00, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0xff6a59cd, __VMLINUX_SYMBOL_STR(i2c_unregister_device) },
	{ 0x8d156e03, __VMLINUX_SYMBOL_STR(i2c_register_driver) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0xd62c833f, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0x94eea794, __VMLINUX_SYMBOL_STR(getnstimeofday64) },
	{ 0xda67b7a5, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0x8e2e8762, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x24ee2945, __VMLINUX_SYMBOL_STR(i2c_master_recv) },
	{ 0xd85cd67e, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0x344b7739, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xfcec0987, __VMLINUX_SYMBOL_STR(enable_irq) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x9a9439, __VMLINUX_SYMBOL_STR(spi_write_then_read) },
	{ 0x9d669763, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xbfe0b852, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0xcef62b6a, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x5a041bb8, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x1cfb04fa, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x7ff3cb0c, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0x5cb474b9, __VMLINUX_SYMBOL_STR(i2c_get_adapter) },
	{ 0xf39cacaa, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xaf286342, __VMLINUX_SYMBOL_STR(i2c_new_device) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("i2c:lzpi_i2c");

MODULE_INFO(srcversion, "C4DF4FD94813BA9289F5070");
