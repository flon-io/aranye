
//
// specifying aranye
//
// Wed Dec 10 10:01:08 JST 2014
//

#include "fara_dom.h"
#include "fara_haml.h"


context "site:"
{
  before each
  {
    fara_node *n = NULL;
  }
  after each
  {
    fara_node_free(n);
  }

  describe "fara_haml_parse_f()"
  {
    it "parses a standalone haml file"
    {
      n = fara_haml_parse_f("../spec/site/%s", "standalone.haml", NULL, NULL);

      expect(fara_node_to_html(n, 1) ===f ""
        "<h1>\n"
        "flying cows\n"
        "</h1>\n"
        "<p>\n"
        "  We've seen flying cows.\n"
        "</p>\n");

      expect(flu_list_get(n->atts, "path") === ""
        "../spec/site/standalone.haml");
    }
  }
}

