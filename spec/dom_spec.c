
//
// specifying aranye
//
// Sun Nov  9 19:38:23 JST 2014
//

#include "fara_dom.h"


context "dom:"
{
  before each
  {
    fara_node *n = NULL;
  }
  after each
  {
    fara_node_free(n);
  }

  describe "fara_node_malloc()"
  {
    it "mallocs a fara node"
    {
      n = fara_node_malloc("div", NULL);

      expect(n != NULL);
      expect(n->parent == NULL);
      expect(n->tag != "div");
      expect(n->tag === "div");
      expect(n->atts->size zu== 0);
      expect(n->children->size zu== 0);
    }
  }

  describe "fara_n()"
  {
    it "mallocs a fara node"
    {
      n = fara_n("div", NULL);

      expect(n != NULL);
      expect(n->parent == NULL);
      expect(n->tag != "div");
      expect(n->tag === "div");
      expect(n->atts->size zu== 0);
      expect(n->children->size zu== 0);
    }
  }

  describe "fara_node_to_html()"
  {
    it "turns a dom node into html"
    {
      expect(0 == 1);
    }
  }
}

