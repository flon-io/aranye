
//
// specifying aranye
//
// Mon Dec  8 13:23:23 JST 2014
//

#include "fara_dom.h"
#include "fara_haml.h"


context "haml:"
{
  before each
  {
    fara_node *n = NULL;
  }
  after each
  {
    fara_node_free(n);
  }

  describe "fara_haml_parse()"
  {
    it "parses a haml line %h1"
    {
      n = fara_haml_parse(
        "%h1");

      expect(fara_node_to_html(n, 1) ===f ""
        "<h1>\n"
        "</h1>\n");
    }

    it "parses a haml line #id.class{ x: y }"
    {
      n = fara_haml_parse(
        "#id.class{ x: y }");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div id=\"id\" class=\"class\" x=\"y\">\n"
        "</div>\n");
    }

    it "parses a haml line .ca.cb"
    {
      n = fara_haml_parse(
        ".ca.cb");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div class=\"ca cb\">\n"
        "</div>\n");
    }

    it "parses a haml hierarchy"
    {
      n = fara_haml_parse(
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

    it "parses a jekyll-esque header"
    {
      n = fara_haml_parse(
        "---\n"
        "layout: my-layout\n"
        "title: xyz\n"
        "---\n"
        "\n"
        "#menu\n"
        "  .about\n"
        "    blah 0\n");

      expect(n->atts != NULL);
      expect(flu_list_get(n->atts, "layout") === "my-layout");
      expect(flu_list_get(n->atts, "title") === "xyz");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div id=\"menu\">\n"
        "  <div class=\"about\">\n"
        "    blah 0\n"
        "  </div>\n"
        "</div>\n");
    }
  }
}

