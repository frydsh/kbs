/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    initwww_all();
    printf("<nobr><center>%s -- ���û�ע��<hr color=\"green\">\n", BBSNAME);
    printf("<font color=\"green\">��ӭ���뱾վ. ������������ʵ��д.</font>");
    printf("<form method=\"post\" action=\"bbsdoreg\">\n");
    printf("<table width=\"600\">\n");
    printf("<tr><td align=\"right\">*���������:</td><td align=\"left\"><input name=\"userid\" size=\"12\" maxlength=\"12\"> (2-12�ַ�, ����ȫΪӢ����ĸ)</td></tr>\n");
    printf("<tr><td align=\"right\">*����������:</td><td align=\"left\"><input type=\"password\" name=\"pass1\" size=\"12\" maxlength=\"12\"> (4-12�ַ�)</td></tr>\n");
    printf("<tr><td align=\"right\">*��ȷ������:</td><td align=\"left\"><input type=\"password\" name=\"pass2\" size=\"12\" maxlength=\"12\"></td></tr>\n");
    printf("<tr><td align=\"right\">*�������ǳ�:</td><td align=\"left\"><input name=\"username\" size=\"20\" maxlength=\"32\"> (2-30�ַ�, ��Ӣ�Ĳ���)</td></tr>\n");
    printf("<tr><td align=\"right\">*������������ʵ����:</td><td align=\"left\"><input name=\"realname\" size=\"20\"> (��������, ����2������)</td></tr>\n");
    printf("<tr><td align=\"right\">*ѧУϵ��������λ:</td><td align=\"left\"><input name=\"dept\" size=\"40\">  (����6���ַ�)</td></tr>\n");
    printf("<tr><td align=\"right\">*��ϸͨѶ��ַ:</td><td align=\"left\"><input name=\"address\" size=\"40\"> (����6���ַ�)</td></tr>\n");
    /*printf("<tr><td align=\"right\">������ʾ��ʽ:</td> ");
       printf("<td align=\"left\"><input type=\"radio\" name=\"xz\" value=\"0\" checked>��ɫ�����Ա� ");
       printf("<input type=\"radio\" name=\"xz\" value=\"1\">��������ɫ ");
       printf("<input type=\"radio\" name=\"xz\" value=\"2\">����ʾ����</td>\n"); */
    printf("<tr><td align=\"right\">����email��ַ(��ѡ):</td><td align=\"left\"><input name=\"email\" size=\"40\"></td></tr>\n");
    printf("<tr><td align=\"right\">��������绰(��ѡ):</td><td align=\"left\"><input name=\"phone\" size=\"40\"></td></tr>\n");
    printf("<tr><td align=\"right\">��վ����(��ѡ):</td><td align=\"left\">");
    printf("<textarea name=\"words\" rows=\"3\" cols=\"40\" wrap=\"physical\"></textarea></td></tr>");
    printf("</table><hr color=\"green\">\n");
    printf("<input type=\"submit\" value=\"�ύ���\"> <input type=\"reset\" value=\"������д\">\n");
    printf("</form></center>");
    http_quit();
}
