#pragma once

#define ARRAY_LENGTH(_arr) (sizeof((_arr))/sizeof((_arr)[0]))
#define STRING_LENGTH(_str) (ARRAY_LENGTH((_str)) - 1)

#define SAFE_FREE(_ptr) do {\
							if((_ptr) != NULL) {\
								free((_ptr));\
								(_ptr) = NULL;\
							}\
						} while(0)

#define SAFE_FCLOSE(_fp) do {\
							if((_fp) != NULL) {\
                                (void)fclose((_fp));\
								(_fp) = NULL;\
							}\
						} while(0)
