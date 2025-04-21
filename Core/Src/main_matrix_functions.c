// ================================================================================
// Main matrix functions C-file
// ================================================================================

#include "main_matrix_functions.h"

uint32_t pattern_tick = 0;
uint8_t brightness_tick = 0, pattern_tick_updated = true;

uint8_t PATTERN_CURR_WAYPOINTS_AMOUNT = 0, PATTERN_OLD_WAYPOINTS_AMOUNT = 0;
uint8_t PATTERN_CURR_WAYPOINTS[8] = {}, PATTERN_OLD_WAYPOINTS[8] = {};

int getPosThroughCoords(int pos_y, int pos_x)
{
  int pos;
  if (pos_y % 2 != 0)
  {
    pos = pos_x + (pos_y - 1)*LED_SIDE_COUNT;
  }
  else
  {
    pos = (LED_SIDE_COUNT + 1 - pos_x) + (pos_y - 1)*LED_SIDE_COUNT;
  }
  pos--;
  return pos;
}

void addCurrPoint(int pos_y, int pos_x)
{
	PATTERN_CURR_WAYPOINTS[PATTERN_CURR_WAYPOINTS_AMOUNT] = getPosThroughCoords(pos_y, pos_x);
	PATTERN_CURR_WAYPOINTS_AMOUNT++;
}

void resetMatrix()
{
    matrix_enabled = !matrix_enabled;
    if (!matrix_enabled)
    {
      //resetMatrixParams();
      for (int i = 0; i < LED_COUNT; i++)
      {
    	  Set_LED(i, 0, 0, 0, 100);
      }
      WS2812_Send();
    }
}

void resetMatrixParams()
{
    pattern_tick = 0;
    brightness_tick = 0;
    pattern_tick_updated = true;
    PATTERN_CURR_WAYPOINTS_AMOUNT = 0;
    PATTERN_OLD_WAYPOINTS_AMOUNT = 0;
}

void playPattern(int patternN)
{
  int num = pattern_tick % (PATTERN_VECTOR_SIZE*2);
  for (int i = 0; i < LED_COUNT; i++)
  {
	  Set_LED(i, 0, 0, 0, 100);
  }

  if (pattern_tick_updated)
  {
	  pattern_tick_updated = false;

	  for (int i = 0; i < PATTERN_CURR_WAYPOINTS_AMOUNT; i++)
	  {
		  PATTERN_OLD_WAYPOINTS[i] = PATTERN_CURR_WAYPOINTS[i];
	  }
	  PATTERN_OLD_WAYPOINTS_AMOUNT = PATTERN_CURR_WAYPOINTS_AMOUNT;
	  PATTERN_CURR_WAYPOINTS_AMOUNT = 0;

	  int pos = num/2;
	  int pos_y = PATTERN_VECTOR_RESIZED[pos][0], pos_x = PATTERN_VECTOR_RESIZED[pos][1];
	  addCurrPoint(pos_y, pos_x);

	  if (pattern_tick != 0)
	  {
	  	if (pos > 0) { pos--; } else { pos = PATTERN_VECTOR_SIZE - 1; }
	  	int prev_pos_y = PATTERN_VECTOR_RESIZED[pos][0], prev_pos_x = PATTERN_VECTOR_RESIZED[pos][1];

		if (!PATTERNS_NOARROW[patternN])
		{
			if (prev_pos_y == pos_y && prev_pos_x != pos_x)
			{
				addCurrPoint(prev_pos_y + 1, prev_pos_x);
				addCurrPoint(prev_pos_y - 1, prev_pos_x);
			}
			else if (prev_pos_y != pos_y && prev_pos_x == pos_x)
			{
				addCurrPoint(prev_pos_y, prev_pos_x + 1);
				addCurrPoint(prev_pos_y, prev_pos_x - 1);
			}
			else
			{
				addCurrPoint(pos_y, prev_pos_x);
				addCurrPoint(prev_pos_y, pos_x);
			}
		}
		else
		{
			addCurrPoint(prev_pos_y, prev_pos_x);
		}
	  }

	  if (PATTERNS_MIRROR[patternN])
	  {
	  	if (PATTERNS_MIRROR_AXIS[patternN])
		{
			for (int i = 1; i <= LED_SIDE_COUNT/2; i++)
			{
				for (int j = 1; j <= LED_SIDE_COUNT; j++)
				{
					for (int k = 0; k < PATTERN_CURR_WAYPOINTS_AMOUNT; k++)
					{
						if (PATTERN_CURR_WAYPOINTS[k] == getPosThroughCoords(i, j))
						{
							addCurrPoint(LED_SIDE_COUNT - i + 1, j);
							break;
						}
					}
				}
			}
		}
		else
		{
			for (int i = 1; i <= LED_SIDE_COUNT; i++)
			{
				for (int j = 1; j <= LED_SIDE_COUNT/2; j++)
				{
					for (int k = 0; k < PATTERN_CURR_WAYPOINTS_AMOUNT; k++)
					{
						if (PATTERN_CURR_WAYPOINTS[k] == getPosThroughCoords(i, j))
						{
							addCurrPoint(i, LED_SIDE_COUNT - j + 1);
							break;
						}
					}
				}
			}
		}
	  }
  }

  uint8_t tick_time = MATRIX_FULL_TICK_TIME * (matrix_time_regime + 1);
  uint8_t brightness_tick_amount = (tick_time / MATRIX_DELAY_TIME);
  if (tick_time %  MATRIX_DELAY_TIME >= tick_time / 2)
  {
	  brightness_tick_amount++;
  }

  for (int i = 0; i < PATTERN_CURR_WAYPOINTS_AMOUNT; i++)
  {
	  Set_LED(PATTERN_CURR_WAYPOINTS[i], 0, 255, 0, (brightness_tick + 1) * 100 / brightness_tick_amount);
  }
  for (int i = 0; i < PATTERN_OLD_WAYPOINTS_AMOUNT; i++)
  {
	  Set_LED(PATTERN_OLD_WAYPOINTS[i], 0, 255, 0, (brightness_tick_amount - brightness_tick - 1) * 100 / brightness_tick_amount);
  }
  WS2812_Send();

  brightness_tick = (brightness_tick + 1) % brightness_tick_amount;
  if (brightness_tick == 0)
  {
	  pattern_tick++;
	  pattern_tick_updated = true;
  }
}
