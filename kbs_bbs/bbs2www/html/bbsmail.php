<?php
	/*	This file dispaly mailbox to user	windinsn nov 7,2003*/
	require("funcs.php");
	
	if ($loginok != 1 || $currentuser[userid] == "guest" )
		html_nologin();
	else{
		html_init("gb2312","","",1);
		if(!strcmp($currentuser["userid"],"guest"))
			html_error_quit("guest û���Լ�������!");
			
		//add custom mail box
		if (isset($_POST["boxname"]))
		{
			$boxname = $_POST["boxname"];
			$ret = bbs_changemaillist(TRUE,$currentuser["userid"],$boxname,0);
			if (!$ret)html_error_quit("�洢�Զ�����������ʧ��!");
			if ($ret > 0)  //��Ŀ������
			{
?>
<SCRIPT language="javascript">
	alert("�Զ����������ѵ�����!������" + <?php echo "\"$ret\"";?>);
	history.go(-1);
</SCRIPT>
<?php
			}
		}	
		//delete custom mail box
		if (isset($_GET["delete"]))
		{
			$delete =$_GET["delete"];  //1-based
			unset($mailboxs[$delete -1]);
			if(!bbs_changemaillist(FALSE,$currentuser["userid"],"",$delete-1))
				html_error_quit("�洢�Զ�����������ʧ��!");
		}	
			
		//system mailboxs
		$mail_box = array(".DIR",".SENT",".DELETED");
		$mail_boxtitle = array("�ռ���","������","������");
		$mail_boxnums = array(bbs_getmailnum2(bbs_setmailfile($currentuser["userid"],".DIR")),bbs_getmailnum2(bbs_setmailfile($currentuser["userid"],".SENT")),bbs_getmailnum2(bbs_setmailfile($currentuser["userid"],".DELETED")));
		//custom mailboxs
		$mail_cusbox = bbs_loadmaillist($currentuser["userid"]);
		$totle_mails = $mail_boxnums[0]+$mail_boxnums[1]+$mail_boxnums[2];
		$i = 2;
		if ($mail_cusbox != -1){
			foreach ($mail_cusbox as $mailbox){
				$i++;
				$mail_box[$i] = $mailbox["pathname"];
				$mail_boxtitle[$i] = $mailbox["boxname"];
				$mail_boxnums[$i] = bbs_getmailnum2(bbs_setmailfile($currentuser["userid"],$mailbox["pathname"]));
				$totle_mails+= $mail_boxnums[$i];
				}
			}
		$mailboxnum = $i + 1;
		$mail_used_space = bbs_getmailusedspace();
		
		bbs_getmailnum($currentuser["userid"],$total,$unread, 0, 0);
		
		
?>
<script language='JavaScript'>
<!--
function bbs_confirm(url,infor){
	if(confirm(infor)){
		window.location.href=url;
		return true;
		}
	return false;
}
-->
</script>
<table width="100%" border="0" cellspacing="0" cellpadding="3">
  <tr> 
    <td class="b2">
	    <a href="bbssec.php" class="b2"><?php echo BBS_FULL_NAME; ?></a>
	    -
	    <?php echo $currentuser["userid"]; ?>������
    </td>
  </tr>
  <tr>
  	<td class="ts2">
  	<?php echo $currentuser["userid"].".bbs@smth.org&nbsp;&nbsp".date("Y-m-d  l"); ?>
  	</td>
  </tr>
  <tr>
  	<td class="b9" align="center" height="50" valign="middle">
  	���������й���<font class="b3"><?php echo $totle_mails; ?></font>���ʼ���ռ�ÿռ�<font class="b3"><?php echo $mail_used_space; ?></font>K
  	</td>
  </tr>
  <tr><td background="/images/dashed.gif" height="9"> </td></tr>
  <tr>
  	<td align="center">
  	<table width="90%" border="0" cellspacing="0" cellpadding="3" class="t1">
  	<tr>
  		<td class="t2">�ļ���</td>
  		<td class="t2">�ż���</td>
  		<td class="t2">���ʼ�</td>
  		<td class="t2">ɾ��</td>
<?php
		for($i=0;$i<$mailboxnum;$i++){
?>
	<tr>	
		<td class="t3"><a href="bbsmailbox.php?path=<?php echo $mail_box[$i];?>&title=<?php echo urlencode($mail_boxtitle[$i]);?>" class="ts2"><?php echo $mail_boxtitle[$i]; ?></a></td>
		<td class="t4"><?php echo $mail_boxnums[$i]; ?></td>
		<td class="t3">
		<?php 
			if($i==0&&$unread!=0){
?>
<img src="/images/nmail.gif" alt="����δ���ʼ�"><?php echo $unread; ?>
<?php 
				}
			else{
?>
<font color="#999999">-</font>
<?php
				}
?>
		</td>
		<td class="t4">
<?php
			if($i>2){
?>
<a href="bbsmail.php?delete=<?php echo $i-2; ?>">ɾ��</a>
<?php
				}
			else{
?>
-
<?php
				}
?>
		</td>
	</tr>	
<?php			
			}
?>
  	<tr>	
		<td class="t3">�ϼ�</td>
		<td class="t4" colspan="3"><?php echo $totle_mails; ?></td>
	</tr>
  	</table>
  	</td>
  </tr>
  <tr>
  	<td align=center>
  	[<a href="bbspstmail.php">�������ʼ�</a>]
  	[<a href="#" onclick="bbs_confirm('bbsmailact.php?act=clear','����������ڵ������ʼ���?')">���������</a>]
  	</td>
  </tr>
 <tr>
  	<td background="/images/dashed.gif" height="9"> </td>
  </tr>
  <form action="<?php echo $_SERVER["PHP_SELF"];	?>" method="POST" onsubmit="
  	if(this.boxname.value==''){
  		alert('���������ļ������ƣ�');
  		return false;
  		}
  	">
  <tr>
  	<td align="center" class="b9">
  	�½��ļ���:
  	<input name="boxname" size=24 maxlength=20 type=text value='' class="b9">
  	<input type="submit" value="���" class="b9">
  	</td>
  </tr>	
  </form>	
</table>
<?php
		html_normal_quit();
		}
?>