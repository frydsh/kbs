/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    int type;
    char pw1[40], pw2[40], pw3[40];
    int i;

    initwww_all();
    if (!loginok)
        http_fatal("����δ��¼, ���ȵ�¼");
    type = atoi(getparm("type"));
    if (type == 0) {
        printf("%s -- �޸����� [�û�: %s]<hr color=\"green\">\n", BBSNAME, getCurrentUser()->userid);
#ifdef SECURE_HTTPS
        printf("<script language=\"JavaScript\">\n");
        printf("<!-- \ndocument.write(\"<form action='https://\"+window.location.hostname+\"/cgi-bin/bbs/bbspwd?type=1' method=post>\"); \n -->\n");
        printf("</script>\n");
#else
        printf("<form action='bbspwd?type=1' method=post>\n");
#endif
        printf("��ľ�����: <input maxlength=\"39\" size=\"12\" type=\"password\" name=\"pw1\"><br>\n");
        printf("���������: <input maxlength=\"39\" size=\"12\" type=\"password\" name=\"pw2\"><br>\n");
        printf("������һ��: <input maxlength=\"39\" size=\"12\" type=\"password\" name=\"pw3\"><br><br>\n");
        printf("<input type=\"submit\" value=\"ȷ���޸�\">\n");
        printf("</form>\n");
        http_quit();
    }
    strsncpy(pw1, getparm("pw1"), sizeof(pw1));
    strsncpy(pw2, getparm("pw2"), sizeof(pw2));
    strsncpy(pw3, getparm("pw3"), sizeof(pw3));
    if (strcmp(pw2, pw3))
        http_fatal("�������벻��ͬ");
    if (strlen(pw2) < 2)
        http_fatal("������̫��");
    if (!checkpasswd2(pw1, getcurrusr()))
        http_fatal("���벻��ȷ");
    /*strcpy(getCurrentUser().passwd, crypt1(pw2, pw2)); */
    setpasswd(pw2, getCurrentUser());
    printf("[%s] �����޸ĳɹ�.", getCurrentUser()->userid);
}
