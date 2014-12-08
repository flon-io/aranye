
//
// specifying aranye
//
// Mon Dec  8 13:23:23 JST 2014
//

#include "fara_dom.h"
#include "fara_haml.h"


context "haml:"
{
  describe "fara_haml_parse()"
  {
    it "parses a haml line"
    {
      fara_node *n = fara_haml_parse(
        "%h1");
    }

    it "parses a haml string into a fara node"
    {
      fara_node *n = fara_haml_parse(
        "#id.class{ x: y }");

      //expect(fara_node_to_html(n, 1) ===f ""
      //  "<div id=\"id\" class=\"class\" x=\"y\">\n"
      //  "</div>");
    }
  }
}

