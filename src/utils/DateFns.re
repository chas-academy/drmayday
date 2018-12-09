[@bs.module "date-fns"] external format: (Js.Date.t, string) => string = "";
[@bs.module "date-fns"]
external distanceInWordsToNow: Js.Date.t => string = "";
[@bs.module "date-fns"]
external distanceInWords: (Js.Date.t, Js.Date.t) => string = "";