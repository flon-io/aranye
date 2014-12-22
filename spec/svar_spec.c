
//
// specifying aranye
//
// Mon Dec 22 11:10:13 JST 2014
//

#include "fara_svar.h"


context "svar:"
{
  before each
  {
    char *r = NULL;
    flu_dict *vars = flu_list_malloc();
  }
  after each
  {
    free(r);
    flu_list_free_all(vars);
  }

  describe "fara_extrapolate()"
  {
    it "understands variable definitions"
    {
      r = fara_extrapolate(rdz_strdup("$alpha: bravo;\n"), vars);

      expect(r === "//$alpha: bravo;\n");
      expect(vars->size i== 1);
      expect(flu_list_get(vars, "$alpha") === "bravo");
    }
  }
}

