<?php
	/*This file shows user tools. windinsn Oct 27,2003*/
	
	require("funcs.php");
	
        $img_subdir = "/images/menuspring/";
        
	function display_board_list($section_names,$section_nums)
	{
?>
<table width="100%" border="0" cellspacing="0" cellpadding="1" class="b1">
<?php
		$i = 0;
		foreach ($section_names as $secname)
		{
			$i++;
			$group=$i-1;
			$group2 = $yank = 0;
			$level = 0;
?>
<tr>
<td align="right" width="16">
<a href="javascript:submenu(0,0,<?php echo $group; ?>,0,0)">
<img id="submenuimg_brd_<?php echo $group; ?>_0" src="/images/close.gif" border="0">
</a>
</td>
<td>
<a href="/bbsboa.php?group=<?php echo $group; ?>" target="f3"><img src="/images/kfolder1.gif" width="16" height="16" border="0" align="absmiddle"><?php echo $secname[0]; ?></a>
</td>
</tr>
<tr id="submenu_brd_<?php echo $group; ?>_0" style="display:none">
<td> </td>
<td id="submenu_brd_<?php echo $group; ?>_0_td">
<DIV></DIV>
</td>
</tr>
<?php
		}
?>
<tr>
<td align="right" width="16">
<img src="/images/open.gif" border="0">
</td>
<td>
<a href="/bbsnewbrd.php" target="f3"><img src="/images/kfolder1.gif" width="16" height="16" border="0" align="absmiddle">�¿�������</a>
</td>
</tr>
</table>
<?php
	}
	
	function display_my_favorite()
	{
?>
<table width="100%" border="0" cellspacing="0" cellpadding="0" class="b1">
<?php
 		$select = -1; 
 		$yank = 0;
 		 		
                if( bbs_load_favboard($select)!=-1 && $boards = bbs_fav_boards($select, 1)) 
                {
			$brd_name = $boards["NAME"]; // Ӣ����
	                $brd_desc = $boards["DESC"]; // ��������
	                $brd_flag = $boards["FLAG"]; 
	                $brd_bid = $boards["BID"];  //�� ID ���� fav dir ������ֵ 
        		$rows = sizeof($brd_name);
                	
                	for ($j = 0; $j < $rows; $j++)	
                        {
				if ($brd_flag[$j]==-1)
				{
?>
<tr>
<td align="right" width="16">
<a href="javascript:submenu(1,<?php echo $brd_bid[$j]; ?>,0,0,0)">
<img id="submenuimg_fav_<?php echo $brd_bid[$j]; ?>" src="/images/close.gif" border="0">
</a>
</td>
<td>
<a href="/bbsfav.php?select=<?php echo $brd_bid[$j]; ?>&up=-1" target="f3"><img src="/images/kfolder1.gif" width="16" height="16" border="0" align="absmiddle"><?php echo $brd_desc[$j]; ?></a>
</td>
</tr>
<tr id="submenu_fav_<?php echo $brd_bid[$j]; ?>" style="display:none">
<td background="/images/line3.gif"> </td>
<td id="submenu_fav_<?php echo $brd_bid[$j]; ?>_td">
<DIV></DIV>
</td>
</tr>
<?php
				}
				else
				{
?>
<tr>
<td width="16" align="right">
<?php			  		
			  		$brd_link="/bbsdoc.php?board=" . urlencode($brd_name[$j]);

					if( $j != $rows-1 )
					{
?>
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<?php
					}
					else
					{
?>
<img src="images/line1.gif" width="11" height="16" align="absmiddle">
<?php
					}
?>
</td><td>
<A href="<?php echo $brd_link; ?>" target="f3"><?php echo $brd_desc[$j]; ?></A>
</td></tr>
<?php
				}
			}
                        bbs_release_favboard(); 
                        
                }
?>
</table>
<?php     
	}
	
	function display_mail_menu($userid)
	{
?>
&nbsp;
<img src="/images/line.gif" border="0" align="absmiddle">
<a href="/bbsnewmail.php" target="f3">�������ʼ�</a><br>
&nbsp;
<img src="/images/line.gif" border="0" align="absmiddle">
<a href="/bbsmailbox.php?path=.DIR&title=<?php echo rawurlencode("�ռ���"); ?>" target="f3">�ռ���</a><br>
&nbsp;
<img src="/images/line.gif" border="0" align="absmiddle">
<a href="/bbsmailbox.php?path=.SENT&title=<?php echo rawurlencode("������"); ?>" target="f3">������</a><br>
&nbsp;
<img src="/images/line.gif" border="0" align="absmiddle">
<a href="/bbsmailbox.php?path=.DELETED&title=<?php echo rawurlencode("������"); ?>" target="f3">������</a><br>
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
		
	function display_blog_menu($userid,$userfirstlogin)
	{
		$db["HOST"]=bbs_sysconf_str("MYSQLHOST");
		$db["USER"]=bbs_sysconf_str("MYSQLUSER");
		$db["PASS"]=bbs_sysconf_str("MYSQLPASSWORD");
		$db["NAME"]=bbs_sysconf_str("MYSQLSMSDATABASE");
		
		@$link = mysql_connect($db["HOST"],$db["USER"],$db["PASS"]) or die("�޷����ӵ�������!");
		@mysql_select_db($db["NAME"],$link);
		
		$query = "SELECT `uid` FROM `users` WHERE `username` = '".$userid."' AND `createtime`  > ".date("YmdHis",$userfirstlogin)." LIMIT 0,1 ;";
		$result = mysql_query($query,$link);
		$rows = mysql_fetch_array($result);
		@mysql_free_result($result);
		if(!$rows)
		{
?>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcapp0.html" target="f3">����BLOG</A><BR>
<?php
		}
		else
		{
?>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/index.php?id=<?php echo $userid; ?>" target="f3">�ҵ�Blog</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcdoc.php?userid=<?php echo $userid; ?>&tag=0" target="f3">������</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcdoc.php?userid=<?php echo $userid; ?>&tag=1" target="f3">������</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcdoc.php?userid=<?php echo $userid; ?>&tag=2" target="f3">˽����</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcdoc.php?userid=<?php echo $userid; ?>&tag=3" target="f3">�ղ���</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcdoc.php?userid=<?php echo $userid; ?>&tag=4" target="f3">ɾ����</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcdoc.php?userid=<?php echo $userid; ?>&tag=5" target="f3">���ѹ���</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcdoc.php?userid=<?php echo $userid; ?>&tag=6" target="f3">�������</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcdoc.php?userid=<?php echo $userid; ?>&tag=7" target="f3">�����趨</A><BR>
&nbsp;
<img src="images/line.gif" width="11" height="16" align="absmiddle">
<A href="/pc/pcmanage.php?userid=<?php echo $userid; ?>&act=post&tag=0&pid=0" target="f3">�������</A><BR>
<?php		
		}	
	}
		
	if ($loginok != 1)
		html_nologin();
	else{
		html_init("gb2312","","",9);

?>
<script src="bbsleft.js"></script>
<body  TOPMARGIN="0" leftmargin="0">
<iframe id="hiddenframe" name="hiddenframe" width="0" height="0"></iframe>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
	<td>
		<table width="100%" border="0" cellspacing="0" cellpadding="0">
		<tr>
			<td colspan="2">
			<img src="/images/t1.gif" border="0">
			</td>
		</tr>
<?php
		if($currentuser["userid"]=="guest")
		{
?>
<form action="/bbslogin.php" method="post" name="form1" target="_top">
<tr>
			<td align="center" width="10%" class="t2" height="25" valign="middle">
			&nbsp;&nbsp;
			<img src="/images/u1.gif" border="0" alt="��¼�û���" align="absmiddle" width="54" height="21">
			</td>
			<td align="left" class="t2">
			<INPUT TYPE=text STYLE="width:80px;height:18px;font-size: 12px;color: #000D3C;border-color: #718BD6;border-style: solid;border-width: 1px;background-color:  #D2E1FE;" LENGTH="10" onMouseOver="this.focus()" onFocus="this.select()" name="id" >
			</td>
</tr>
<tr>
			<td align="center" width="10%" class="t2" height="25" valign="middle">
			&nbsp;&nbsp;
			<img src="/images/u3.gif" border="0" alt="�û�����" align="absmiddle" width="54" height="21">
			</td>
			<td align="left" class="t2">
			<INPUT TYPE=password  STYLE="width:80px;height:18px;font-size: 12px;color: #000D3C;border-color: #718BD6;border-style: solid;border-width: 1px;background-color:  #D2E1FE;" LENGTH="10" name="passwd" maxlength="39">
			</td>
</tr>
<tr>
			<td align="center" width="10%" colspan="2" class="t2" height="25" valign="middle">
			<input type="image" name="login" src="/images/l1.gif" alt="��¼��վ">
			<a href="/bbsreg0.html" target="_top"><img src="/images/l3.gif" border="0" alt="ע�����û�"></a>
			</td>
</tr>
</form>
<?php
		}
		else
		{
?>
		<tr>
			<td align="center" width="10%" class="t2" height="25" valign="middle">
			&nbsp;&nbsp;
			<img src="/images/u1.gif" border="0" alt="��¼�û���" align="absmiddle" width="54" height="21">
			</td>
			<td align="left" class="t2">
			&nbsp;&nbsp;
			<?php	echo $currentuser["userid"];	?>
			</td>
		</tr>
<?php
		}
?>
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
			<td><a href="<?php echo MAINPAGE_FILE; ?>" target="f3"><img src="<?php echo $img_subdir; ?>m1.gif" border="0" alt="��ҳ" align="absmiddle"> ��ҳ����</a></td>
		</tr>
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><a href="/cgi-bin/bbs/bbs0an" target="f3"><img src="<?php echo $img_subdir; ?>m2.gif" border="0" alt="������" align="absmiddle"> ������</a></td>
		</tr>
		<tr>
			<td width="16">
				<DIV class="r" id="divboarda">
				<a href='javascript:changemn("board");'><img id="imgboard" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td><a href="/bbssec.php" target="f3"><img src="<?php echo $img_subdir; ?>mfolder0.gif" border="0" alt="����������" align="absmiddle"> ����������</a></td>
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
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<form action="cgi-bin/bbs/bbssel" target="f3">
			<td>
			<img src="<?php echo $img_subdir; ?>m5.gif" border="0" alt="����������" align="absmiddle">
			<input name="board" type="text" class="f2" value="����������" size="12" onclick="this.value=''" /> 
<input name="submit" type="submit" value="GO" style="width:25px;height:20px;font-size: 12px;color: #ffffff;border-style: none;background-color: #718BD6;" />
			</td>
			</form>
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
			<td><a href="bbsfav.php?select=-1" target="f3"><img src="<?php echo $img_subdir; ?>mfolder3.gif" border="0" alt="�ҵ��ղؼ�" align="absmiddle"> �ҵ��ղؼ�</a></td>
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
<?php
	if (defined("HAVE_PC"))
	{
?>
		<tr>
			<td width="16">
				<DIV class="r" id="divpca">
				<a href='javascript:changemn("pc");'><img id="imgpc" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='/pc/pcmain.php' target='f3'>
			<img src="<?php echo $img_subdir; ?>m3.gif" border="0" alt="<?php echo BBS_FULL_NAME; ?>Blog" align="absmiddle"> ˮľBlog
			</a>
			</td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divpc">
<?php
		if($currentuser["userid"]!="guest")
			display_blog_menu($currentuser["userid"],$currentuser["firstlogin"]);
?>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pcmain.php" target="f3">Blog��ҳ</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pc.php" target="f3">�û��б�</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pcrec.php" target="f3">�Ƽ�����</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pclist.php" target="f3">��������</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pcsec.php" target="f3">����Ŀ¼</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pcnew.php" target="f3">��������</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pcnew.php?t=c" target="f3">��������</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pcsearch2.php" target="f3">�ļ�����</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/pc/pcnsearch.php" target="f3">��������</a><br>
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/bbsdoc.php?board=SMTH_blog" target="f3">Blog��̳</a><br>
					&nbsp;
					<img src="/images/line1.gif" border="0" align="absmiddle">
					<a href="/pc/index.php?id=SYSOP" target="f3">��������</a><br>
				</DIV>
			</td>
		</tr>
		
<?php
	} // defined(HAVE_PC)
?>
<?php
	if($currentuser["userid"]!="guest"){
?>
		<tr>
			<td width="16">
				<DIV class="r" id="divmaila">
				<a href='javascript:changemn("mail");'><img id="imgmail" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td><a href="/bbsmail.php" target="f3"><img src="<?php echo $img_subdir; ?>m4.gif" border="0" alt="�ҵ�����" align="absmiddle"> �ҵ�����</a></td>
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
				<DIV class="r" id="divchata">
				<a href='javascript:changemn("chat");'><img id="imgchat" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='javascript:changemn("chat");'>
			<img src="<?php echo $img_subdir; ?>m8.gif" border="0" alt="̸��˵��" align="absmiddle"> ̸��˵��
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
<?php
	if($currentuser["userid"]!="guest")
	{
?>
		<tr>
			<td width="16">
				<DIV class="r" id="divtoola">
				<a href='javascript:changemn("tool");'><img id="imgtool" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='javascript:changemn("tool");'>
			<img src="<?php echo $img_subdir; ?>m7.gif" border="0" alt="���˲�������" align="absmiddle"> ���˲�������
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
<?php
	}
?>
<!--
		<tr>
			<td width="16">
				<DIV class="r" id="divstylea">
				<a href='javascript:changemn("style");'><img id="imgstyle" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td><img src="<?php echo $img_subdir; ?>m7.gif" border="0" alt="������" align="absmiddle"> ������</td>
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
			<td width="16">
				<DIV class="r" id="divexpa">
				<a href='javascript:changemn("exp");'><img id="imgexp" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='javascript:changemn("exp");'>
			<img src="<?php echo $img_subdir; ?>m6.gif" border="0" alt="ˮľ�ؿ�Web��" align="absmiddle"> ˮľ�ؿ�Web��
			</a>
			</td>
		</tr>
		<tr>
			<td> </td>
			<td>
				<DIV class="s" id="divexp">
					&nbsp;
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/express/1103/smth_express.htm" target="f3">2003��11�º�</a><br>
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
		<tr>
			<td width="16">
				<DIV class="r" id="divsera">
				<a href='javascript:changemn("ser");'><img id="imgser" src="/images/close.gif" border="0"></a>
				</DIV>
			</td>
			<td>
			<a href='javascript:changemn("ser");'>
			<img src="<?php echo $img_subdir; ?>m7.gif" border="0" alt="�ļ����ؼ�����" align="absmiddle"> �ļ����ؼ�����
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
					<img src="/images/line.gif" border="0" align="absmiddle">
					<a href="/data/fterm-smth.zip" target="_blank">Fterm����</a><br>
				<?php /*
					&nbsp;
					<img src="/images/line1.gif" border="0" align="absmiddle">
					<a href="/data/FeedDemon-rc4a.exe" target="_blank">FeedDemon����</a><br>
				*/ ?>
				</DIV>
			</td>
		</tr>
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><a href="telnet:smth.org"><img src="<?php echo $img_subdir; ?>m6.gif" border="0" alt="telnet��¼" align="absmiddle"> Telnet��¼</a>
		</tr>
<!--
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><img src="/images/t10.gif" border="0" alt="�鿴������Ϣ" align="absmiddle"> �鿴����
		</tr>
-->
		<tr>
			<td width="16"><img src="/images/open.gif" border="0"></td>
			<td><a href="/bbslogout.php" target="_top"><img src="<?php echo $img_subdir; ?>m7.gif" border="0" alt="�뿪��վ" align="absmiddle"> �뿪��վ</a>
		</tr>
		
		</table>
	</td>
</tr>
</table>
<script type="text/javascript" src="/bbsleftad.js"> </script>
<?php
		
		html_normal_quit();
		}
?>
