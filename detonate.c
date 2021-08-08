/* $Id$ */

#include "php.h"
#include "ext/standard/info.h"


#define EVAL_CALLBACK_FUNCTION  "__eval"


static zend_op_array* (*old_compile_string)(zval *source_string, char *filename TSRMLS_DC);


static zend_op_array* detonate_compile_string(zval *source_string, char *filename TSRMLS_DC)
{
	zend_op_array *op_array = NULL;
	int op_compiled = 0;

	if(strstr(filename, "eval()'d code")) {
		if(zend_hash_str_exists(CG(function_table), EVAL_CALLBACK_FUNCTION, strlen(EVAL_CALLBACK_FUNCTION) TSRMLS_CC)) {
			zval function;
			zval retval;
			zval parameter[2];

			parameter[0] = *source_string;

			ZVAL_STRING(&function, EVAL_CALLBACK_FUNCTION);
			ZVAL_STRING(&parameter[1], filename);

			if(call_user_function(CG(function_table), NULL, &function, &retval, 2, parameter TSRMLS_CC) == SUCCESS) {
				switch(Z_TYPE(retval)) {
					case IS_STRING:
						op_array = old_compile_string(&retval, filename TSRMLS_CC);
					case IS_FALSE:
						op_compiled = 1;
						break;
				}
			}
			
			zval_dtor(&function);
			zval_dtor(&retval);
			zval_dtor(&parameter[1]);
		}
	}

	if(op_compiled) {
		return op_array;
	} else {
		return old_compile_string(source_string, filename TSRMLS_CC);
	}
}


PHP_MINIT_FUNCTION(detonate)
{
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(detonate)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(detonate)
{
	old_compile_string = zend_compile_string;
	zend_compile_string = detonate_compile_string;
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(detonate)
{
	zend_compile_string = old_compile_string;
	return SUCCESS;
}

PHP_MINFO_FUNCTION(detonate)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "SuperUserek - detonate", "enabled");
	php_info_print_table_row(2, "callback function", EVAL_CALLBACK_FUNCTION);
	php_info_print_table_end();
}


zend_function_entry detonate_functions[] = {
	ZEND_FE_END
};

zend_module_entry detonate_module_entry = {
	STANDARD_MODULE_HEADER,
	"detonate",
	detonate_functions,
	PHP_MINIT(detonate),
	PHP_MSHUTDOWN(detonate),
	PHP_RINIT(detonate),	
	PHP_RSHUTDOWN(detonate),
	PHP_MINFO(detonate),
	"0.0.1-dev",
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(detonate)

