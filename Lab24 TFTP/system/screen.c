#include <lfb.h>

#include <inttypes.h>

typedef uint32_t u32;

int ScreenInit()
{
return 0;
}

void ScreenClose()
{
}
void ScreenClear()
{
}

void DisplayCharacter(char ch, u32 color)
{
fbputchar(ch);
}

int DisplayString(const char *string, int length, u32 color)
{
  int result = 0;

  // Loop until there is no more length in the string
  for (;length; --length)
  {
    // Return length displayed if string NULL character is found
    if (*string == '\0')
      return result;

    // Display the character and move to the next in string
    DisplayCharacter(*string++, color);
    result++;
  }

  // Return the amount of characters displayed
  return result;
}
