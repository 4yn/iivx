# iivx
Beatmania IIDX and Sound Voltex hybrid controller running on Arduino Due

## Hardware

- Beatmania Buttons x9
  - Beatmania layout requires two rows of 3 and 4 vertical keys + 2 utility buttons
  - Voltex layout requires two rows of 4 and 2 (horizontal) keys + 1 utility button
  - Total 9 buttons, top row 3 vertical, middle row 4 vertical and bottom row 2 horizontal
  - [9x White Buttons purchased via Taobao, $10 SGD w/o shipping](https://item.taobao.com/item.htm?spm=a230r.1.14.22.b82vYR&id=15772881563&ns=1&abbucket=2#detail), [(Search Listings)](https://item.taobao.com/item.htm?spm=a230r.1.14.22.b82vYR&id=15772881563&ns=1&abbucket=2#detail)
- Rotary Encoders x3
  - Beatmania layout requires 1 turntable, KOCs use an optical encoder, but a rotary encoder is used here for consistency in purchasing
  - Voltex layout requires 2 knobs for VOL-L and VOL-R control
  - 24ppr encoders should suffice to give a resolution of 96 points per 360 degrees, but higher resolution encoders (300, 600 ppr) are accounted for in this project
  - [3x 600ppr Encoders purchased via Taobao, $25 SGD w/o shipping](http://world.taobao.com/item/16341987368.htm?spm=a312a.7700714.0.0.kKKUs0#detail), [(Search Listings)](http://world.taobao.com/search/search.htm?_ksTS=1446028667153_50&spm=2015090702&_input_charset=utf-8&navigator=all&json=on&q=%E6%97%8B%E8%BD%AC%E7%BC%96%E7%A0%81%E5%99%A8&callback=__jsonp_cb&cna=ovtdDnkvbxQCAWDYnK9OEKJS&abtest=_AB-LR854-PV854_2099)
- Microcontroller
  - Arduino DUE used due to:
    - High clock rate
    - Interrupt capability on all pins (other Arduino boards only have 2-4 interrupt pins, insufficient for 4x reading of 3 different rotary encoders which need at least 6 interrupt pins)
    - Native USB functionality, custom HID descriptor is used
  - Purchased a clone from Shenzhen Hua Qiang Bei, $20 SGD


TODO: In-Program Documentation