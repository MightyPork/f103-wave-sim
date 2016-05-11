#!/usr/bin/env php
<?php

$map = file_get_contents('main.map');

$at_flash = strpos($map, '.text           0x0000000008000000');
$at_ram   = strpos($map, '.data           0x0000000020000000');
$at_dbg   = strpos($map, '.memory_b1_text');

$discard = substr($map, 0, $at_flash);
$flash = substr($map, $at_flash, $at_ram - $at_flash);
$ram = substr($map, $at_ram, $at_dbg - $at_ram);

$flash = str_replace("lib/gcc/arm-none-eabi/5.3.0/../../../../", "", $flash);

file_put_contents("main.flash.map", $flash);
file_put_contents("main.ram.map", $ram);

//preg_match("/\*\(\.eh_frame\)\n\s*0x00000000(2[0-9a-f]+)\s*\. = ALIGN \(0x4\)/i", $ram, $m);

//echo "Free RAM: " . (0x20010000 - hexdec($m[1])) . " B\n";
