<?php


$needlogin=1;

require("inc/funcs.php");
require("inc/user.inc.php");
require("inc/ubbcode.php");

preprocess();

setStat("�쿴�û���Ϣ");

show_nav();

if (isErrFounded()) {
	html_error_quit() ;
} else {
	?>
	<br>
	<TABLE cellSpacing=0 cellPadding=0 width=97% border=0 align=center>
	<?php

	if ($loginok==1) {
		showUserMailbox();
?>
</table>
<?php
	}
	head_var();
	showUserData($user,$user_num);
}

//showBoardSampleIcons();
show_footer();

function preprocess() {
	global $user,$user_num;
	$userarray=array();
	if (($user_num=bbs_getuser($_GET['id'],$userarray))==0) {
		foundErr("�����û�����ʧ�ܣ�");
		return false;
	}
	$user=$userarray;
	return true;

}

function showUserData($user, $user_num) {
require("inc/userdatadefine.inc.php");
$flag=1<<29;
if ($user['userdefine'] & $flag) {
?>
<table width=97% border=0 cellspacing=0 cellpadding=3 align=center>
  <tr> 
    <td><img src="<?php
	if ($user['userface_img']==-2) {
		echo $user['userface_url'];
	} else {
		echo 'userface/image'.$user['userface_img'].'.gif';
	}
?>" width=<?php echo $currentuser['userface_width'];  ?> height=<?php echo $currentuser['userface_height'];  ?> align=absmiddle> 
<b><?php echo $user['userid']; ?></b> 
</td>
    <td align=right>
��ǰλ�ã�[���������б�]<img src=pic/zhuangtai.gif width=16 height=16 align=absmiddle> 
      ״̬��
����  [���ߣ�0Mins]
  </td>
  </tr>
</table>

<table cellspacing=1 cellpadding=3 align=center  style="table-layout:fixed;word-break:break-all" class=TableBorder1>
  <col width=20% ><col width=*><col width=40% > 
  <tr> 
    <th colspan=2 align=left height=25>��������</th>
    <td rowspan=7 align=center class=TableBody1 width=40% valign=top>
<?php
	$photo_url=htmlspecialchars(trim($user['photo_url']),ENT_QUOTES);
	if ($photo_url!='') {
		echo "<img src='".$photo_url."'>"; 
	} else {
		echo  "<font color=gray>��</font>";
	}
?>
    </td>
  </tr>   
  <tr> 
    <td class=TableBody1 width=20% align=right>�� ��</td>
    <td class=TableBody1><?php echo chr($user['gender'])=='M'?'��':'Ů'; ?> </td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right>�� ����</td>
    <td class=TableBody1>
<?php
	echo get_astro($user['birthmonth'],$user['birthday']);
?></td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right>�� �ѣ�</td>
    <td class=TableBody1>
	<?php echo showIt($user['OICQ']); ?>
</td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>�ɣãѣ�</td>
    <td class=TableBody2>
	<?php echo showIt($user['ICQ']); ?>
</td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right>�ͣӣΣ�</td>
    <td class=TableBody1>
	<?php echo showIt($user['MSN']); ?>
 </td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>�� ҳ��</td>
    <td class=TableBody2>
	<?php 
	$homepage=htmlspecialchars(trim($user['homepage']),ENT_QUOTES);
	if ($homepage!='') {
		echo '<a href="'.$homepage.'" target="_blank">'.$homepage.'</a>'; 
	} else {
		echo "<font color=gray>δ֪</font>";
	}
	?>
</td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right valign=top>&nbsp;</td>
    <td class=TableBody1>&nbsp;</td>
    <td class=TableBody1 align=center width=40% >
      <b><a href="javascript:openScript('messanger.asp?action=new&touser=admin',500,400)">��������</a> | <a href="friendlist.asp?action=addF&myFriend=admin" target=_blank>��Ϊ����</a></b></td>
  </tr>
</table>
<br>
<?php
}
$flag=1<<30;
if ($user['userdefine'] & $flag) {
?>
<table cellspacing=1 cellpadding=3 align=center class=TableBorder1 style="table-layout:fixed;word-break:break-all">
  <col width=20% ><col width=*><col width=40% > 
  <tr> 
    <th colspan=2 align=left height=25>
      �û���ϸ����</th>
    <td rowspan=16 class=TableBody1 width=40% valign=top>
<b>�ԣ�����</b>
<br>
<?php   echo $character[$user['character']]; ?>
<br><br><br>
<b>���˼�飺</b><br>
<?php   
	$filename=bbs_sethomefile($user["userid"],"plans");
	if (is_file($filename)) {
		$plans = bbs_printansifile($filename);
		$v_plans = split ( "<br />", $plans );
		$num = count ( $v_plans );

		$plans = "";

		for ( $i=0; $i<$num && $i<20 ; $i++ ){
			$plans .= $v_plans[$i];
			$plans .= "<br />";
		}
		echo dvbcode($plans,0);
	} else {
		echo "<font color=gray>����һ������ʲôҲû������^_^</font>";
	}
?>
<br>
</td>
  </tr>   
  <tr> 
    <td class=TableBody1 width=20% align=right>��ʵ������</td>
    <td class=TableBody1><?php echo showIt($user['realname']);	?></td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>�������ң�</td>
    <td class=TableBody2><?php echo showIt($user['country']); ?> </td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>�� ����</td>
    <td class=TableBody2>
<?php
	if ( ($user['birthyear']!=0) && ($user['birthmonth']!=0) && ($user['birthday']!=0)) {
		echo '19'.$user['birthyear'].'��'.$user['birthmonth'].'��'.$user['birthday'].'��';
	} else {
		echo "<font color=gray>δ֪</font>";
	}?>
 </td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right>ʡ�����ݣ�</td>
    <td class=TableBody1><?php echo showIt($user['province']); ?></td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>�ǡ����У�</td>
    <td class=TableBody2><?php  echo showIt($user['city']); ?></td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right>��ϵ�绰��</td>
    <td class=TableBody1>	<?php echo showIt($user['telephone']); ?></td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>ͨ�ŵ�ַ��</td>
    <td class=TableBody2><?php   echo showIt($user['address']); ?></td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>�ţ����죺</td>
    <td class=TableBody2>
	<?php 
	$reg_email=htmlspecialchars(trim($user['reg_email']),ENT_QUOTES);
	if ($reg_email!='') {
		echo '<a href=mailto:'.$reg_email.'>'.$reg_email.'</a>'; 
	} else {
		echo "<font color=gray>δ֪</font>";
	}
	?>
</td>
  </tr>

  <tr> 
    <td class=TableBody1 width=20% align=right>������Ф��</td>
    <td class=TableBody1><?php echo showIt($shengxiao[$user['shengxiao']]); ?> </td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>Ѫ�����ͣ�</td>
    <td class=TableBody2><?php    echo showIt($bloodtype[$user['bloodtype']]); ?></td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right>�š�������</td>
    <td class=TableBody1><?php    echo showIt($religion[$user['religion']]) ?></td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>ְ����ҵ��</td>
    <td class=TableBody2><?php    echo showIt($profession[$user['profession']]); ?></td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right>����״����</td>
    <td class=TableBody1><?php    echo showIt($married[$user['married']]); ?></td>
  </tr>
  <tr> 
    <td class=TableBody2 width=20% align=right>���ѧ����</td>
    <td class=TableBody2><?php    echo showIt($education[$user['education']]); ?></td>
  </tr>
  <tr> 
    <td class=TableBody1 width=20% align=right>��ҵԺУ��</td>
    <td class=TableBody1><?php    echo showIt($user['graduateschool']); ?></td>
  </tr></table>
<br>
<?php
}
?>
<table cellspacing=1 cellpadding=3 align=center class=TableBorder1>
  <tr>
    <th align=left colspan=6 height=25> ��̳����</th>
  </tr>
  <tr>
    <td class=TableBody1 width=15% align=right>���֣�</td>

    <td  width=35%  class=TableBody1><b><?php echo $user['score']; ?> </b></td>
    <td width=15% align=right class=TableBody1>�������ӣ�</td>
    <td width=35%  class=TableBody1> <b>N/A</b>ƪ</td>
  </tr>
  <tr> 
    <td class=TableBody1 width=15% align=right>����ֵ��</td>
    <td  width=35%  class=TableBody1><b>N/A </b></td>
    <td width=15% align=right class=TableBody1>����������</td>
    <td width=35%  class=TableBody1><b><?php echo $user['numposts']; ?></b> ƪ</td>
  </tr>
  <tr> 
    <td class=TableBody1 width=15% align=right>��̳�ȼ���</td>
    <td  width=35%  class=TableBody1><b><?php echo bbs_getuserlevel($user['userid']); ?> </b></td>
    <td width=15% align=right class=TableBody1>��ɾ���⣺</td>
    <td width=35%  class=TableBody1><b><font color=#FF0000>N/A</font></b> 
      ƪ</td>
  </tr>
  <tr> 
    <td class=TableBody1 width=15% align=right>����ֵ��</td>
    <td  width=35%  class=TableBody1><b><font color=#FF0000>N/A</font> </b></td>
    <td width=15% align=right class=TableBody1>��ɾ���ʣ�</td>
<td width=35%  class=TableBody1><b></b> <font color=#FF0000><b>
N/A
</b></font> 
    </td>
  </tr>
  <tr> 
    <td class=TableBody1 width=15% align=right>��  �ɣ�</td>
    <td  width=35%  class=TableBody1><b>
<?php echo showIt($groups[$user['group']]); ?>
 </b></td>
    <td class=TableBody1 width=15% align=right>��½������</td>
    <td width=35%  class=TableBody1><b><?php echo $user['numlogins']; ?></b> 
    </td>
  </tr>
  <tr> 
    <td class=TableBody1 width=15% align=right>ע�����ڣ�</td>
    <td  width=35%  class=TableBody1><b><?php echo strftime("%Y-%m-%d %H:%M:%S", $user['firstlogin']); ?></b></td>
    <td width=15% align=right class=TableBody1>�ϴε�¼��</td>
    <td width=35%  class=TableBody1><b><?php echo strftime("%Y-%m-%d %H:%M:%S", $user['lastlogin']); ?></b></td>
  </tr>
</table>
<br>
<table cellspacing=1 cellpadding=3 align=center class=TableBorder1>
  <tr> 
    <th align=left colspan=4>
      �ʲ����</th>
  </tr>
  <tr> 
    <td class=TableBody1 width=15% align=right>�ֽ���ң�</td>
    <td width=35%  class=TableBody1><b><?php echo $user['money']; ?></b></td>
    <td colspan=2 valign=top rowspan=4 class=TableBody1>��ְ̳��
      <hr size=1 width=100 align=left>
<?php echo bbs_getuserlevel($user['userid']); ?><br>
      </td>
  </tr>
  <tr> 
    <td class=TableBody1 width=15% align=right>��Ʊ��ֵ��</td>
    <td  width=35%  class=TableBody1><b>N/A</b></td>
  </tr>

  <tr> 
    <td class=TableBody1 width=15% align=right>���д�</td>
    <td width=35%  class=TableBody1><b>N/A</b></td>
  </tr>
  <tr> 
    <td class=TableBody1 width=15% align=right>�� �� ����</td>
    <td width=35%  class=TableBody1><b>N/A</b></td>
  </tr>
</table>
<br>

<table class=TableBorder1 cellspacing=1 cellpadding=3 align=center>
<tr><th height="25" align=left colspan=2>��ݹ���ѡ��</th></tr>

<tr><td class=TableBody1 height=25 colspan=2>
<B>�û�����ѡ��</B>��   �� <a href=# onclick="alert('�ù������ڿ����С�');" title=�������û���������½�ͷ���>����</a> | <a href=# onclick="alert('�ù������ڿ����С�');" title=���θ��û�����̳�ķ���>����</a> | <a href=# onclick="alert('�ù������ڿ����С�');" title=������û�����̳������������>���</a> | <a href=# onclick="alert('�ù������ڿ����С�');" title=���û����з�ֵ����>����</a> | <a href=# onclick="alert('�ù������ڿ����С�');">�༭���û���̳Ȩ��</a> ��
</td></tr>



<tr><td class=TableBody1 valign=middle height=25 colspan=2>
<B>�û��������IP</B>��   <a href=# onclick="alert('�ù������ڿ����С�');"><?php echo $user['lasthost']; ?></a>&nbsp;&nbsp;���IP�鿴�û���Դ������
</td></tr>

</table>

<?php
}

?>