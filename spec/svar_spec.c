
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
    flu_list_free_all(vars);
  }

  describe "fara_extrapolate()"
  {
    it "understands variable definitions"
    {
      r = fara_extrapolate("$alpha: bravo;\n", vars);

      expect(r == NULL);
      expect(vars->size i== 1);
      expect(flu_list_get(vars, "$alpha") === "bravo");
    }

    it "extrapolates vars"
    {
      r = fara_extrapolate("$color: white;\n", vars); free(r);
      r = fara_extrapolate("color: $color;\n", vars);

      expect(r ===f "color: white;\n");
    }

    it "extrapolates in var definitions"
    {
      r = fara_extrapolate("$ab: cd;\n", vars); free(r);
      r = fara_extrapolate("$ef: gh-$ab;\n", vars);

      expect(r == NULL);
      expect(vars->size i== 2);
      expect(flu_list_get(vars, "$ab") === "cd");
      expect(flu_list_get(vars, "$ef") === "gh-cd");
    }

    it "extrapolates unknown vars to \"\""
    {
      r = fara_extrapolate("x: $nada;\n", vars);

      expect(r ===f "x: ;\n");
    }

    it "removes // comments after definitions"
    {
      r = fara_extrapolate("$x: y z // blah blah\n", vars);

      expect(r == NULL);
    }

    it "removes // comments after regular css"
    {
      r = fara_extrapolate("  color: blue; // blah blah\n", vars);

      expect(r ===f "  color: blue; \n");
    }

    it "removes // comment lines"
    {
      r = fara_extrapolate(" // blah blah\n", vars);

      expect(r == NULL);
    }

    it "doesn't touch /* comments"
    {
      r = fara_extrapolate(
        "  /* http://meyerweb.com/eric/tools/css/reset/\n", vars);

      expect(r ===f ""
        "  /* http://meyerweb.com/eric/tools/css/reset/\n");

      r = fara_extrapolate(
        "*/\n", vars);

      expect(r ===f ""
        "*/\n");
    }

    it "is ok with */EOL"
    //{
    //  r = fara_extrapolate("*/", vars);
    //  expect(r ===f "*/\n");
    //}
  }
}

