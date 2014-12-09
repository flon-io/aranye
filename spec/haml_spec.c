
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
    it "parses a haml line %h1"
    {
      fara_node *n = fara_haml_parse(
        "%h1");

      expect(fara_node_to_html(n, 1) ===f ""
        "<h1>\n"
        "</h1>\n");
    }

    it "parses a haml line #id.class{ x: y }"
    {
      fara_node *n = fara_haml_parse(
        "#id.class{ x: y }");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div id=\"id\" class=\"class\" x=\"y\">\n"
        "</div>\n");
    }

    it "parses a haml line .ca.cb"
    {
      fara_node *n = fara_haml_parse(
        ".ca.cb");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div class=\"ca cb\">\n"
        "</div>\n");
    }

    it "parses a haml hierarchy"
    {
      fara_node *n = fara_haml_parse(
        "#menu\n"
        "  .about\n"
        "    blah 0\n"
        "  .links\n"
        "    blah 1");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div id=\"menu\">\n"
        "  <div class=\"about\">\n"
        "    blah 0\n"
        "  </div>\n"
        "  <div class=\"links\">\n"
        "    blah 1\n"
        "  </div>\n"
        "</div>\n");
    }
  }
}

