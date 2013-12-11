#include <glib.h>
#include "libgdbr.h"

char parse_cmd1[] = "+$qSupported:multiprocess+;qRelocInsn+#2a$T02thread:01;#04+";

void parse_test() {
	libgdbr_t instance;
	instance.read_buff = parse_cmd1;
	instance.data_len = strlen(parse_cmd1);
	libgdbr_parse_packet(&instance);
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);
	g_assert_cmpstr("Test", ==, "Test");
	g_test_add_func("/set1/parser test", parse_test);
	return g_test_run();
}
