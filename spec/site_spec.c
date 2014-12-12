
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
    }

    it "extrapolates a layout"
    {
      n = fara_haml_parse_f("../spec/site/%s", "index.haml", NULL, NULL);

      //flu_putf(fara_node_to_st(n, 1));

      expect(fara_node_to_html(n, 1) ===f ""
        "<!DOCTYPE html>\n"
        "<html>\n"
        "  <head>\n"
        "    <title>\n"
        "carpe diem\n"
        "    </title>\n"
        "  </head>\n"
        "  <body>\n"
        "    <div class=\"header\">\n"
        "      header\n"
        "    </div>\n"
        "    <h1>\n"
        "carpe diem\n"
        "    </h1>\n"
        "    <p>\n"
        "  This is our site. We crafted it thanks to Santa's elves.\n"
        "    </p>\n"
        "    <p>\n"
        "  It sucks.\n"
        "    </p>\n"
        "    <div class=\"footer\">\n"
        "      footer\n"
        "    </div>\n"
        "  </body>\n"
        "</html>\n");
    }

    it "includes"
    {
      n = fara_haml_parse_f("../spec/site/layinc.haml", NULL, NULL);

      //flu_putf(fara_node_to_st(n, 1));

      expect(fara_node_to_html(n, 1) ===f ""
        "<!DOCTYPE html>\n"
        "<html>\n"
        "  <head>\n"
        "    <title>\n"
        "layout and include\n"
        "    </title>\n"
        "  </head>\n"
        "  <body>\n"
        "    <div class=\"header\">\n"
        "      header\n"
        "    </div>\n"
        "    <h1>\n"
        "layout and include\n"
        "    </h1>\n"
        "    <p>\n"
        "  We pride in our quality products.\n"
        "    </p>\n"
        "    <div class=\"advertisement\">\n"
        "      <hr>\n"
        "      </hr>\n"
        "      <p>\n"
        "    And now, a word from our sponsor...\n"
        "      </p>\n"
        "      <hr>\n"
        "      </hr>\n"
        "    </div>\n"
        "    <p>\n"
        "  We don't care about typos though.\n"
        "    </p>\n"
        "    <div class=\"footer\">\n"
        "      footer\n"
        "    </div>\n"
        "  </body>\n"
        "</html>\n");
    }

    it "includes html"
    {
      n = fara_haml_parse_f("../spec/site/layincht.haml", NULL, NULL);

      //flu_putf(fara_node_to_st(n, 1));

      expect(fara_node_to_html(n, 1) ===f ""
        "<!DOCTYPE html>\n"
        "<html>\n"
        "  <head>\n"
        "    <title>\n"
        "layout and html include\n"
        "    </title>\n"
        "  </head>\n"
        "  <body>\n"
        "    <div class=\"header\">\n"
        "      header\n"
        "    </div>\n"
        "    <h1>\n"
        "layout and html include\n"
        "    </h1>\n"
        "    <p>\n"
        "  We pride in our quality products.\n"
        "    </p>\n"
        "<div class=\"advertisement\">\n"
        "  <hr/>\n"
        "  <p>\n"
        "    This web page is brought to you by Goat Milk Inc.\n"
        "  </p>\n"
        "  <hr/>\n"
        "</div>\n"
        "\n"
        "    <p>\n"
        "  We don't care about typos though.\n"
        "    </p>\n"
        "    <div class=\"footer\">\n"
        "      footer\n"
        "    </div>\n"
        "  </body>\n"
        "</html>\n");
    }
  }
}

