# A nice!view

The nice!view is a low-power, high refresh rate display meant to replace I2C OLEDs traditionally used.

This shield requires that an `&nice_view_spi` labeled SPI bus is provided with _at least_ MOSI, SCK, and CS pins defined.

## Animation Timeout Option

A config option is available to stop animations after a configurable timeout to preserve battery. This feature requires animations to be enabled (`CONFIG_NICE_VIEW_GEM_ANIMATION=y`). To enable the timeout feature, add to your shield config:

```
CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT=y
CONFIG_NICE_VIEW_GEM_ANIM_TIMEOUT_SECONDS=10  # Optional, default is 10 seconds
```

By default, the timeout feature is disabled. When enabled, animations will stop after the specified number of seconds of inactivity and resume when any key is pressed.
