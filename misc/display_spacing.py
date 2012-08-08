panel_width = 61.0 # cm
panel_height = 122.0 # cm


# mid panel = 9 wide
leds_across = 9
leds_down = 12
led_separation = panel_width / leds_across

print "%.2f cm separation" % led_separation

print "VERTICAL"
print "vertically %.2f cm from center of top led to center of bottom led" % (led_separation * (leds_down - 1))
top_margin = 12 # cm
for led in range(leds_down):
    ypos = top_margin + led * led_separation
    print "row %d: %.1f from top" % (led, ypos)

print
print "HORIZONTAL"
drill = 1.2 # 12mm drill
for led in range(leds_across):
    xpos = led_separation / 2 + led_separation * led
    print "column %d: %.1f (hole edges at %.2f and %.2f)" % (led, xpos, xpos-drill/2, xpos+drill/2)

