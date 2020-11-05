

// filtering output
void Filter(){
  if (co2ppm > 0 && co2ppm < 10200) Fco2ppm = co2ppm;
  if (sihummidity <= 100 && sihummidity > 0) Fsihummidity = sihummidity;
  if (sitemp < 130 && sitemp > -41) Fsitemp = sitemp;
  if (cssco2ppm <= 7500 && cssco2ppm > 400) Fcssco2ppm = cssco2ppm; //total max is 8200, but it worse looking
  if (csstvoc < 1000 && csstvoc > 0) Fcsstvoc = csstvoc; //total max is 1187, and looks bad
                                                         //anyway it over the comfort zone... very much...
  }

