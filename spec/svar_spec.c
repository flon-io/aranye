
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
      r = fara_extrapolate("$alpha: bravo;\n", vars);

      expect(r === "/*$alpha: bravo;*/\n");
      expect(vars->size i== 1);
      expect(flu_list_get(vars, "$alpha") === "bravo");
    }

    it "extrapolates vars"
    {
      r = fara_extrapolate("$color: white;\n", vars); free(r);
      r = fara_extrapolate("color: $color;\n", vars);

      expect(r === "color: white;\n");
    }

    it "extrapolates in var definitions"
    {
      r = fara_extrapolate("$ab: cd;\n", vars); free(r);
      r = fara_extrapolate("$ef: gh-$ab;\n", vars);

      expect(r === "/*$ef: gh-cd;*/\n");
      expect(vars->size i== 2);
      expect(flu_list_get(vars, "$ab") === "cd");
      expect(flu_list_get(vars, "$ef") === "gh-cd");
    }

    it "extrapolates unknown vars to \"\""
    {
      r = fara_extrapolate("x: $nada;\n", vars);

      expect(r === "x: ;\n");
    }
  }
}

