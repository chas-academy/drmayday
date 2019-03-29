open Emotion;

let light_background = "FAFDFF";

let dark_text = "00364D";
let light_text = "5990A6";

let danger_light = "FFCCD8";
let danger_dark = "990025";

let warning_light = "FFF0CC";
let warning_dark = "996B00";

let h1 = [%css
  [
    color(`hex(dark_text)),
    fontFamily("Rubik"),
    fontSize(`px(24)),
    fontWeight(500),
    margin4(`px(0), `px(0), `px(24), `px(0)),
  ]
];

let p_lead = [%css
  [
    color(`hex(dark_text)),
    fontSize(`px(18)),
    fontWeight(700),
    lineHeight(`pct(150.0)),
    margin(`px(0)),
  ]
];

let p_quiet = [%css
  [
    color(`hex(light_text)),
    fontSize(`px(14)),
    fontWeight(400),
    lineHeight(`pct(100.0)),
    margin(`px(0)),
  ]
];

let small_bold = [%css
  [fontSize(`px(12)), fontWeight(700), lineHeight(`pct(100.0))]
];
