<?php
	/*This file shows user tools. windinsn Oct 27,2003*/
	
	require("funcs.php");
	
	function display_board_group($section_names,$section_nums,$group_name,$group_id,$totle_group,$group,$group2,$level){
		$yank = 0;
		settype($group, "integer");
		settype($group2, "integer");
		settype($yank, "integer");
		if ($group < 0)
			$group = 0;
		if ($group <= sizeof($section_nums)){
			$boards = bbs_getboards($section_nums[$group], $group2, $yank);
			$brd_name = $boards["NAME"]; // Ӣ����
			$brd_desc = $boards["DESC"]; // ��������
			$brd_flag = $boards["FLAG"]; //flag
			$brd_bid = $boards["BID"]; //flag
			$rows = sizeof($brd_name);			
?>
<table width="100%" border="0" cellspacing="0" cellpadding="1" class="b1">
<tr> 
	<td width="16" align="right">
	<DIV class=r id=divb<?php echo $level.$group_id; ?>a>
	<A href='javascript:changemn("b<?php echo $level.$group_id; ?>");'>
	<img id="imgb<?php echo $level.$group_id; ?>" src="images/close.gif" width="16" height="16" border="0" align="absmiddle"> 
	</A></DIV>
	</td>
	<td>
	<a href="/bbsboa.php?group=<?php echo $group; ?>" target="f3"><img src="images/folder1.gif" width="16" height="16" border="0" align="absmiddle"><?php echo $group_name; ?></a>
	</td>
</tr>
<tr>
<?php
	if($group_id != $totle_group - 1){
?>
	<td  width="16" background="/images/line3.gif"> </td>
<?php
	}
	else{
?>
	<td width="16"></td>
<?php
	}
?>
	<td class="b1">
	<DIV class=s id=divb<?php echo $level.$group_id; ?>>
	
<?php
			for ($j = 0; $j < $rows; $j++)	
			{
				if ($brd_flag[$j]&BBS_BOARD_GROUP){
					$brd_link="/bbsboa.php?group=" . $group . "&group2=" . $brd_bid[$j];
					$level++;
					display_board_group($section_names,$section_nums,$brd_desc[$j],$j,$rows,$group,$brd_bid[$j],$level);
					$level--;
					}
				else{
			  		$brd_link="/bbsdoc.php?board=" . urlencode($brd_name[$j]);

				if( $j != $rows-1 )
				{
?>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<?php
				}
				else
				{
?>
&nbsp;
<img src="images/line1.gif" width="11" height="16" align="absmiddle">
<?php
				}
?>
<A href="<?php echo $brd_link; ?>" target="f3"><?php echo $brd_desc[$j]; ?></A><BR>
<?php
				}
			}
?>
</DIV>
</td>
</tr>
</table>
<?php		
		}
	}
	
	
	
	function display_board_list($section_names,$section_nums){
		$i = 0;
		foreach ($section_names as $secname){
			$i++;
			$group=$i-1;
			$group2 = $yank = 0;
			$level = 0;
			display_board_group($section_names,$section_nums,$secname[0],$group,count($section_names),$group,$group2,$level);
			}
		}
	
	
	function display_fav_group($boards,$group_name,$group_id,$totle_group,$level,$up=-1){
		$brd_name = $boards["NAME"]; // Ӣ����
		$brd_desc = $boards["DESC"]; // ��������
		$brd_flag = $boards["FLAG"]; //flag
		$brd_bid = $boards["BID"]; 
		$rows = sizeof($brd_bid);
			
?>
<table width="100%" border="0" cellspacing="0" cellpadding="1" class="b1">
<tr> 
	<td width="16" align="right"> 
	<DIV class=r id=divf<?php echo $level.$group_id; ?>a><A href='javascript:changemn("f<?php echo $level.$group_id; ?>");'> 
	<img id="imgf<?php echo $level.$group_id; ?>" src="images/close.gif" width="16" height="16" border="0" align="absmiddle"> 
	</A></DIV>
	</td>
	<td>
	<img src="images/folder1.gif" width="16" height="16" border="0" align="absmiddle"><a href="bbsfav.php?select=<?php echo $level; ?>&up=<?php echo $up; ?>" target="f3"><?php echo $group_name; ?></a>
	</td>
</tr>
<tr>
<?php
	if($group_id != $totle_group - 1){
?>
	<td background="/images/line3.gif"> </td>
<?php
	}
	else{
?>
	<td> </td>
<?php
	}
?>
	<td>
	<DIV class=s id=divf<?php echo $level.$group_id; ?>>
<?php
			for ($j = 0; $j < $rows; $j++)	
			{
				if ($brd_flag[$j]&BBS_BOARD_GROUP){
					if( bbs_load_favboard($brd_bid[$j])!=-1 && $fav_boards = bbs_fav_boards($brd_bid[$j], 1) && $brd_bid[$j]!= -1) {
	                                	$fav_boards = bbs_fav_boards($brd_bid[$j], 1);
	                                	display_fav_group($fav_boards,$brd_desc[$j],$j,$rows,$brd_bid[$j],$level);
	                                	}
	                                }
				else{
			  		$brd_link="/bbsdoc.php?board=" . urlencode($brd_name[$j]);

				if( $j != $rows-1 )
				{
?>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<?php
				}
				else
				{
?>
&nbsp;
<img src="images/line1.gif" width="11" height="16" align="absmiddle">
<?php
				}
?>
<A href="<?php echo $brd_link; ?>" target="f3"><?php echo $brd_desc[$j]; ?></A><BR>
<?php
				}
			}
?>
</DIV>
</td>
</tr>
</table>
<?php		
	}
	
	
	
	function display_my_favorite(){
 		$select = -1; 
 		$yank = 0;
 		 		
                if( bbs_load_favboard($select)!=-1 && $boards = bbs_fav_boards($select, 1)) {
			$brd_name = $boards["NAME"]; // Ӣ����
	                $brd_desc = $boards["DESC"]; // ��������
	                $brd_flag = $boards["FLAG"]; 
	                $brd_bid = $boards["BID"];  //�� ID ���� fav dir ������ֵ 
        		$rows = sizeof($brd_name);
                	
                	for ($j = 0; $j < $rows; $j++)	
                        {
				if ($brd_flag[$j]==-1){//&BBS_BOARD_GROUP){
					$fav_boards = bbs_fav_boards($brd_bid[$j], 1); 
	                                display_fav_group($fav_boards,$brd_desc[$j],$j,$rows,$brd_bid[$j]);
					}
				else{
			  		$brd_link="/bbsdoc.php?board=" . urlencode($brd_name[$j]);

				if( $j != $rows-1 )
				{
?>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<?php
				}
				else
				{
?>
&nbsp;
<img src="images/line1.gif" width="11" height="16" align="absmiddle">
<?php
				}
?>
<A href="<?php echo $brd_link; ?>" target="f3"><?php echo $brd_desc[$j]; ?></A><BR>
<?php
				}
			}
                        bbs_release_favboard(); 
                        
                	}
               
	}
	
	function display_mail_menu($userid)
	{
?>
&nbsp;
<img src="/images/line.gif" border="0" align="absmiddle">
<a href="/bbsnewmail.php" target="f3">�������ʼ�</a><br>
&nbsp;
<img src="/images/line.gif" border="0" align="absmiddle">
<a href="/bbsmailbox.php?path=.DIR&title=�ռ���" target="f3">�ռ���</a><br>
&nbsp;
<img src="/images/line.gif" border="0" align="absmiddle">
<a href="/bbsmailbox.php?path=.SENT&title=������" target="f3">������</a><br>
&nbsp;
<img src="/images/line.gif" border="0" align="absmiddle">
<a href="/bbsmailbox.php?path=.DELETED&title=������" target="f3">������</a><br>
<?php
		//custom mailboxs
		$mail_cusbox = bbs_loadmaillist($userid);
		if ($mail_cusbox != -1)
		{
			foreach ($mail_cusbox as $mailbox)
			{
				echo "&nbsp;\n".
					"<img src=\"/images/line.gif\" border=\"0\" align=\"absmiddle\">\n".
					"<a href=\"/bbsmailbox.php?path=".$mailbox["pathname"]."&title=".urlencode($mailbox["boxname"])."\" target=\"f3\">".htmlspecialchars($mailbox["boxname"])."</a><br>\n";
			}
		}
?>
&nbsp;
<img src="/images/line1.gif" border="0" align="absmiddle">
<a href="/bbspstmail.php" target="f3">�����ʼ�</a>
<?php		
	}
		
		
	if ($loginok != 1)
		html_nologin();
	else{
		html_init("gb2312","","",9);

?>
<script src="bbsleft.js"></script>
<body>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
	<td>
		<table width="100%" border="0" cellspacing="0" cellpadding="0">
		<tr>
			<td colspan="2" background="/images/back1.gif">
			<img src="/images/t1.gif" border="0">
			</td>
		</tr>
		<tr>
			<td align="center" width="10%" class="t2" height="25" valign="middle">
			&nbsp;&nbsp;
			<img src="/images/u1.gif" border="0" alt="��¼�û���" align="absmiddle" width="54" height="21">
			</td>
			<td align="left" class="t2">
			&nbsp;&nbsp;
			<?php	
				if($currentuser["userid"]=="guest")
					echo "�οͲι�";
				else
					echo $currentuser["userid"];	?>
			</td>
		</tr>
		<tr>
			<td colspan="2" class="t2">
			<img src="/images/t2.gif" border="0">
			</td>
		</tr>
		</table>
	</td>
</tr>
<tr>
	<td height="5"> </td>
</tr>
<tr>
	<td align="center">
		<table width="90%" border="0" cellspacing="0" cellpadding="1" class="b1">
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><a href="/mainpage.html" target="f3"><img src="/images/home.gif" border="0" alt="��ҳ" align="absmiddle"> ��ҳ����</a></td>
		</tr>
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><a href="/cgi-bin/bbs/bbs0an" target="f3"><img src="/images/t3.gif" border="0" alt="����������" align="absmiddle"> ����������</a></td>
		</tr>
		<tr>
			<td width="16">
				<DIV class="r" id="divexpa">
				<a href='javascript:changemn("exp");'><img id="imgexp" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='javascript:changemn("exp");'>
			<img src="/images/t7.gif" border="0" alt="ˮľ�ؿ�Web��" align="absmiddle"> ˮľ�ؿ�Web��
			</a>
			</td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divexp">
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/express/0903/smth_express.htm" target="f3">2003��9�º�</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/express/0703/smth_express.htm" target="f3">2003��7�º�</a><br>
					&nbsp;
					<img src="/images/line1.gif" border="0" align="absmiddle">
					<a href="/express/0603/smth_express.htm" target="f3">2003��6�º�</a><br>
				</DIV>
			</td>
		</tr>
		
<?php
	if($currentuser["userid"]!="guest"){
		if (bbs_getmailnum($currentuser["userid"],$total,$unread, 0, 0)) {
			if( $unread != 0 ){
?>
<script>alert('�������ż�!')</script>
<?php
			}
		}
?>
		<tr>
			<td width="16">
				<DIV class="r" id="divtoola">
				<a href='javascript:changemn("tool");'><img id="imgtool" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='javascript:changemn("tool");'>
			<img src="/images/t4.gif" border="0" alt="���˹�����" align="absmiddle"> ���˹�����
			</a>
			</td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divtool">
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsfillform.html" target="f3">��дע�ᵥ</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsinfo.php" target="f3">��������</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="bbsplan.php" target="f3">��˵����</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="bbssig.php" target="f3">��ǩ����</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="cgi-bin/bbs/bbspwd" target="f3">�޸�����</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="cgi-bin/bbs/bbsparm" target="f3">�޸ĸ��˲���</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsal.php" target="f3">ͨѶ¼</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsrsmsmsg.php" target="f3">���Ź�����</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsnick.php" target="f3">��ʱ���ǳ�</a><br>
					&nbsp;
					<img src="/images/line1.gif" border="0" align="absmiddle">
					<a href="/bbsfall.php" target="f3">�趨����</a><br>
				</DIV>
			</td>
		</tr>
		<tr>
			<td width="16">
				<DIV class="r" id="divmaila">
				<a href='javascript:changemn("mail");'><img id="imgmail" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td><a href="/bbsmail.php" target="f3"><img src="/images/t5.gif" border="0" alt="�����ż�" align="absmiddle"> �����ż�</a></td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divmail">
<?php
	display_mail_menu($currentuser["userid"]);
?>					
				</DIV>
			</td>
		</tr>
<?php
		}
?>
		<tr>
			<td width="16">
				<DIV class="r" id="divboarda">
				<a href='javascript:changemn("board");'><img id="imgboard" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td><a href="/bbssec.php" target="f3"><img src="/images/folder4.gif" border="0" alt="����������" align="absmiddle"> ����������</a></td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divboard">
<?php
	display_board_list($section_names,$section_nums);
?>
				</DIV>
			</td>
		</tr>
<?php
	if($currentuser["userid"]!="guest"){
?>
		<tr>
			<td width="16">
				<DIV class="r" id="divfava">
				<a href='javascript:changemn("fav");'><img id="imgfav" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td><a href="bbsfav.php?select=-1" target="f3"><img src="/images/folder3.gif" border="0" alt="���˶�����" align="absmiddle"> ���˶�����</a></td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divfav">
<?php
	display_my_favorite();
?>
				</DIV>
			</td>
		</tr>
<?php
		}
?>
		<tr>
			<td width="16">
				<DIV class="r" id="divchata">
				<a href='javascript:changemn("chat");'><img id="imgchat" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='javascript:changemn("chat");'>
			<img src="/images/t6.gif" border="0" alt="̸��˵��" align="absmiddle"> ̸��˵��
			</a>
			</td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divchat">
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">&nbsp;
					<a href="bbsuser.php" target="f3">�����û�</a><br>
<?php
	if($currentuser["userid"]=="guest"){
?>					
					&nbsp;
					<img src="/images/line1.gif" border="0" align="absmiddle">
					<a href="/bbsqry.php" target="f3">��ѯ����</a>
<?php
		}					
	else{
?>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsqry.php" target="f3">��ѯ����</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsfriend.php" target="f3">���ߺ���</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbssendsms.php" target="f3">���Ͷ���</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsmsg.php" target="f3">�鿴����ѶϢ</a><br>
					&nbsp;
					<img src="/images/line1.gif" border="0" align="absmiddle">
					<a href="/bbssendmsg.php" target="f3">����ѶϢ</a>
<?php
		}
?>	
				</DIV>
			</td>
		</tr>
<!--
		<tr>
			<td width="16">
				<DIV class="r" id="divstylea">
				<a href='javascript:changemn("style");'><img id="imgstyle" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td><img src="/images/t7.gif" border="0" alt="������" align="absmiddle"> ������</td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divstyle">
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsstyle.php?s=0">������</a><br>
					&nbsp;
					<img src="/images/line1.gif" border="0" align="absmiddle">
					<a href="/bbsstyle.php?s=1">С����</a><br>
				</DIV>
			</td>
		</tr>
-->
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<form action="cgi-bin/bbs/bbssel" target="f3">
			<td>
			<img src="/images/t8.gif" border="0" alt="ѡ��������" align="absmiddle">
			<input name="board" type="text" class="f2" value="ѡ��������" size="12" onmouseover="this.focus()" onfocus="this.select()"> 
			</td>
			</form>
		</tr>
		<tr>
			<td width="16">
				<DIV class="r" id="divsera">
				<a href='javascript:changemn("ser");'><img id="imgser" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='javascript:changemn("ser");'>
			<img src="/images/t9.gif" border="0" alt="ϵͳ��Ѷ�ͷ���" align="absmiddle"> ϵͳ��Ѷ�ͷ���
			</a>
			</td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divser">
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/games/index.html" target="f3">��������</a><br>
					&nbsp;
					<img src="/images/line1.gif" border="0" align="absmiddle">
					<a href="/data/fterm-smth.zip" target="_blank">Fterm����</a><br>
				</DIV>
			</td>
		</tr>
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><a href="telnet:smth.org"><img src="/images/t11.gif" border="0" alt="telnet��¼" align="absmiddle"> Telnet��¼</a>
		</tr>
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><img src="/images/t10.gif" border="0" alt="�鿴������Ϣ" align="absmiddle"> �鿴����
		</tr>
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><a href="/bbslogout.php" target="_top"><img src="/images/leave.gif" border="0" alt="�뿪��վ" align="absmiddle"> �뿪��վ</a>
		</tr>
		
		</table>
	</td>
</tr>
</table>
<p align="center"><a href="http://www.dawning.com.cn/" target="_blank"><img src="/images/dawning.gif" width="120" height="53" border="0" alt="��⹫˾"></a></p>
<?php
		html_normal_quit();
		}
?>
