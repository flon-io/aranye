
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
      n = fara_node_malloc("div", flu_list_malloc());

      expect(n != NULL);
      expect(n->parent == NULL);
      //expect(n->t != "div");
      expect(n->t === "div");
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
      //expect(n->t != "div");
      expect(n->t === "div");
      expect(n->atts->size zu== 0);
      expect(n->children->size zu== 0);
    }
  }

  describe "fara_t()"
  {
    it "mallocs a fara text node"
    {
      n = fara_t("hello, %s", "world");

      expect(n != NULL);
      expect(n->parent == NULL);
      expect(n->t === "hello, world");
      expect(n->atts == NULL);
      expect(n->children == NULL);
    }
  }

  describe "fara_node_push()"
  {
    it "adds a child to a node"
    {
      n = fara_n("div", "class", "a", NULL);
      fara_node *n1 = fara_n("div", "class", "b", NULL);

      fara_node_push(n, n1);

      expect(n->children->first != NULL);
      expect(n->children->first->item == n1);
      expect(n1->parent == n);
    }
  }

  describe "fara_node_to_html()"
  {
    it "renders a dom node as html"
    {
      n = fara_n("div", "class", "menu-%s", "bretzel", NULL);

      expect(fara_node_to_html(n, 0) ===f ""
        "<div class=\"menu-bretzel\"></div>");
    }

    it "renders a node and its children as html"
    {
      n = fara_n("div", "class", "a", NULL);
      fara_node_push(n, fara_n("div", "class", "b", NULL));

      expect(fara_node_to_html(n, 0) ===f ""
        "<div class=\"a\"><div class=\"b\"></div></div>");
    }

    it "renders a 'document' node"
    {
      n = fara_node_malloc(NULL, NULL);
      fara_node_push(n, fara_n("div", "class", "a", NULL));
      fara_node_push(n, fara_n("div", "class", "b", NULL));

      expect(fara_node_to_html(n, 1) ===f ""
        "<div class=\"a\">\n"
        "</div>\n"
        "<div class=\"b\">\n"
        "</div>\n");
    }

    context "when FARA_F_INDENT"
    {
      it "renders as indented html"
      {
        n = fara_n("div", "class", "a", NULL);
        fara_node_push(n, fara_n("div", "class", "b", NULL));

        expect(fara_node_to_html(n, FARA_F_INDENT) ===f ""
          "<div class=\"a\">\n"
          "  <div class=\"b\">\n"
          "  </div>\n"
          "</div>");
      }
    }
  }
}

