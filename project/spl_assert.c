#include "main.h"
#include "utils/timebase.h"
#include "com/debug.h"

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @return None
  */
void __attribute__((noreturn))
assert_failed(uint8_t* file, uint32_t line)
{
	error("Assert failed in file %s, line %"PRIu32".", file, line);

	/* Infinite loop */
	while (1);
}

#endif

