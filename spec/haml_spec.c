
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

  describe "fara_haml_parse_s()"
  {
    it "parses %h1"
    {
      n = fara_haml_parse_s(
        "%h1");

      expect(fara_node_to_html(n, 1) ===f ""
        "<h1>\n"
        "</h1>\n");
    }

    it "parses %body.site-layout"
    {
      n = fara_haml_parse_s(
        "%body.site-layout");

      expect(fara_node_to_html(n, 1) ===f ""
        "<body class=\"site-layout\">\n"
        "</body>\n");
    }

    it "parses #id.class{ x: y }"
    {
      n = fara_haml_parse_s(
        "#id.class{ x: y }");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div id=\"id\" class=\"class\" x=\"y\">\n"
        "</div>\n");
    }

    it "parses .x{ y: \"\" }"
    {
      n = fara_haml_parse_s(
        ".x{ y: \"\" }");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div class=\"x\" y=\"\">\n"
        "</div>\n");
    }

    it "parses .ca.cb"
    {
      n = fara_haml_parse_s(
        ".ca.cb");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div class=\"ca cb\">\n"
        "</div>\n");
    }

    it "accepts attributes with quotes"
    {
      n = fara_haml_parse_s(
        ".x{ y: 'z', a: 'b c' }");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div class=\"x\" y=\"z\" a=\"b c\">\n"
        "</div>\n");
    }

    it "accepts attributes with double-quotes"
    {
      n = fara_haml_parse_s(
        ".x{ y: \"z\", a: \"b c\" }");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div class=\"x\" y=\"z\" a=\"b c\">\n"
        "</div>\n");
    }

    it "parses a haml hierarchy"
    {
      n = fara_haml_parse_s(
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
      n = fara_haml_parse_s(
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

    it "finishes an element before going with the next text"
    {
      n = fara_haml_parse_s(
        "#menu\n"
        "  .stuff\n"
        "    blah 0\n"
        "  nada\n"
        "");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div id=\"menu\">\n"
        "  <div class=\"stuff\">\n"
        "    blah 0\n"
        "  </div>\n"
        "  nada\n"
        "</div>\n");
    }

    it "tolerates handlebars"
    {
      n = fara_haml_parse_s(
        ".hidden\n"
        "  #optionTemplate\n"
        "    .option\n"
        "      {{ name }}\n"
        "    .values\n"
        "      {{ #each values }}\n"
        "      .value{ data-id: \"{{ id }}\" }\n"
        "        {{ name }}\n"
        "      {{ /each }}\n"
        "");

      expect(fara_node_to_html(n, 1) ===f ""
        "<div class=\"hidden\">\n"
        "  <div id=\"optionTemplate\">\n"
        "    <div class=\"option\">\n"
        "      {{ name }}\n"
        "    </div>\n"
        "    <div class=\"values\">\n"
        "      {{ #each values }}\n"
        "      <div class=\"value\" data-id=\"{{ id }}\">\n"
        "        {{ name }}\n"
        "      </div>\n"
        "      {{ /each }}\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n");
    }

    it "leverages document headers"
    {
      n = fara_haml_parse_s(
        "---\n"
        "title: cave canem\n"
        "---\n"
        "\n"
        "%h1= title\n"
        ".nada\n"
        "  lore ipsum nada\n");

      expect(n->atts != NULL);
      expect(flu_list_get(n->atts, "title") === "cave canem");

      expect(fara_node_to_html(n, 1) ===f ""
        "<h1>\n"
        "cave canem\n"
        "</h1>\n"
        "<div class=\"nada\">\n"
        "  lore ipsum nada\n"
        "</div>\n");
    }

    it "understands !!!"
    {
      n = fara_haml_parse_s(
        "!!!\n"
        "%h1\n"
        "  big title\n");

      expect(fara_node_to_html(n, 1) ===f ""
        "<!DOCTYPE html>\n"
        "<h1>\n"
        "  big title\n"
        "</h1>\n");
    }
  }
}

