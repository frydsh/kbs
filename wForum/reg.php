<?php
	$needlogin=0;
	require("inc/funcs.php");
	$stats="���û�ע��";
		show_nav();
	@$action=$_POST['action'];
	if ($action=='apply') {
		$stats="��д����";
		do_apply();
	} elseif ($action=='save') {
		$stats="�ύע��";
		do_save();
	} else {
		$stats="ע��Э��";
		do_show();
	}

show_footer();

function do_show() {
?>
<table cellpadding=3 cellspacing=1 align=center class=tableborder1>
    <tr><th align=center><form action="reg.php" method=post>�������������</td></tr>
	<input type="hidden" name="action" value="apply">
    <tr><td class=tablebody1 align=left>
<?php	require("inc/reg_txt.php") ; ?>
	</td></tr>
    <tr><td align=center class=tablebody2><input type=submit value=��ͬ��></td></form></tr>
</table>
<?php


}

function do_apply(){
	global $SiteName;
?>

<form method=post action="reg.php">
<input type="hidden" name="action" value="save">
<table cellpadding=3 cellspacing=1 align=center class=tableborder1>
<thead>
<Th colSpan=2 height=24><?php echo $SiteName; ?> -- ���û�ע��</Th>
</thead>
<TBODY> 
<TR> 
<TD width=40% class=tablebody1><B>����</B>��<BR>2-12�ַ�������Ӣ����ĸ�����֣����ַ���������ĸ</TD>
<TD width=60%  class=tablebody1> 
<input name=userid size=12 maxlength=12></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>����</B>��<BR>���������룬5-39�ַ������ִ�Сд��<BR>
�벻Ҫʹ���κ����� '*'��' ' �� HTML �ַ�</TD>
<TD width=60%  class=tablebody1> 
<input type=password name=pass1 size=12 maxlength=12></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>����</B>��<BR>������һ��ȷ��</TD>
<TD width=60%  class=tablebody1> 
<input type=password name=pass2 size=12 maxlength=12></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>�ǳ�</B>��<BR>����BBS�ϵ��ǳƣ�2-39�ַ�����Ӣ�Ĳ���</TD>
<TD width=60%  class=tablebody1> 
<input name=username size=20 maxlength=32></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>��ʵ����</B>��<BR>��������, ����2������</TD>
<TD width=60%  class=tablebody1> 
<input name=realname size=20></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>�Ա�</B>��<BR>��ѡ�������Ա�</TD>
<TD width=60%  class=tablebody1> <INPUT type=radio CHECKED value=1 name=gender>
<IMG  src=pic/Male.gif align=absMiddle>�к� &nbsp;&nbsp;&nbsp;&nbsp; 
<INPUT type=radio value=0 name=gender>
<IMG  src=pic/Female.gif align=absMiddle>Ů��</font></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>����</B>��<BR>��������</TD>
<TD width=60%  class=tablebody1> 
<input name=year size=4 maxlength=4>��<input name=month size=2 maxlength=2>��<input name=day size=2 maxlength=2>��<br></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>Email</B>��<BR>������Ч�����ʼ���ַ</TD>
<TD width=60%  class=tablebody1> 
<input name=email size=40></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>ѧУϵ��������λ</B>��<BR>�������ģ�����6���ַ�</TD>
<TD width=60%  class=tablebody1> 
<input name=dept size=40></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>��ϸͨѶ��ַ</B>��<BR>�������ģ�����6���ַ�</TD>
<TD width=60%  class=tablebody1> 
<input name=address size=40></TD>
</TR>
<TR> 
<TD width=40% class=tablebody1><B>����绰</B>��<BR>��������绰����д������</TD>
<TD width=60%  class=tablebody1> 
<input name=phone size=40> </TD>
</TR>
<tr>
<td colspan=2 align=center>
<input type=submit value=�ύ���>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <input type=reset value=������д>
</td>
</tr>
</table>
</form>
<?php
}

function do_save(){
	global $SiteName;
	@$userid=$_POST["userid"];
	@$pass1=$_POST["pass1"];
	@$pass2=$_POST["pass2"];
	@$nickname=$_POST["username"];

	@$realname=$_POST["realname"];
	@$dept=$_POST["dept"];
    @$address=$_POST["address"];
	@$year=$_POST["year"];
	@$month=$_POST["month"];
	@$day=$_POST["day"];
	@$email=$_POST["email"];
	@$phone=$_POST["phone"];
	@$gender=$_POST["gender"];


	if(strcmp($pass1,$pass2))
		html_error_quit("�����������벻һ��");
	else if(strlen($pass1) < 5 || !strcmp($pass1,$userid))
       	html_error_quit("���볤��̫�̻��ߺ��û�����ͬ!");

	$ret=bbs_createnewid($userid,$pass1,$nickname);
	switch($ret)
	{
	case 0:
			break;
	case 1:
			html_error_quit("�û����з�������ĸ�ַ��������ַ�������ĸ!");
			break;
	case 2:
			html_error_quit("�û�������Ϊ������ĸ!");
			break;
	case 3:
			html_error_quit("ϵͳ���ֻ�������!");
			break;
	case 4:
			html_error_quit("���û����Ѿ���ʹ��!");
			break;
	case 5:
			html_error_quit("�û���̫��,�12���ַ�!");
			break;
	case 6:
			html_error_quit("����̫��,�39���ַ�!");
			break;
	case 10:
			html_error_quit("ϵͳ����,����ϵͳ����ԱSYSOP��ϵ.");
			break;
	default:
			html_error_quit("ע��IDʱ����δ֪�Ĵ���!");
			break;
	}
	if($gender!='1')$gender=2;
    settype($year,"integer");
	settype($month,"integer");
	settype($day,"integer");
	$ret=bbs_createregform($userid,$realname,$dept,$address,$gender,$year,$month,$day,$email,$phone,FALSE);//�Զ�����ע�ᵥ

	switch($ret)
	{
	case 0:
		break;
	case 2:
		html_error_quit("���û�������!");
		break;
	case 3:
		html_error_quit("��������");
		break;
	default:
		html_error_quit("δ֪�Ĵ���!");
		break;
	}
?>
<table cellpadding=3 cellspacing=1 align=center class=tableborder1>
<tr>
<th height=24>ע�ᵥ�ѳɹ���<?php echo $SiteName; ?>��ӭ���ĵ���</th>
</tr>
<tr><td class=tablebody1><br>
<ul>
<li>�����ڻ�û��ͨ�������֤��,ֻ���������Ȩ�ޣ����ܷ��ġ����š�����ȡ�</li>
<li>ϵͳ���Զ�����ע�ᵥ����վ�����ͨ����,�㽫��úϷ��û�Ȩ�ޣ�</li>
<li><a href="index.asp">����������</a></li></ul>
</td></tr>
</table>
<?php
}

?>

</body>
</html>
