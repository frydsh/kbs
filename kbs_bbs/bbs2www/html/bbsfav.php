<?php
        /**
         * This file lists fav boards to user.  @author caltary
         */
        require("funcs.php");
        if ($loginok !=1 )
                html_nologin();
        else
        {
                html_init("gb2312");
                if (isset($_GET["select"]))
                        $select = $_GET["select"];
                else
                        $select = 0;
                settype($select, "integer");
                if (isset($_GET["up"]))
                        $list_father=$_GET["up"];
                else
                        $list_father=0;
                settype($list_father,"integer");

                if (!strcmp($currentuser["userid"],"guest"))
                    html_error_quit("����ע���ʺ�");
                
                if ($select < -1)// || $group > sizeof($section_nums))
                        html_error_quit("����Ĳ���");
                if(bbs_load_favboard($select)==-1)
                        html_error_quit("����Ĳ���");
                if (isset($_GET["delete"]))
                {
                        $delete_s=$_GET["delete"];
                        settype($delete_s,"integer");
                        bbs_del_favboard($delete_s);
                }
                if (isset($_GET["dname"]))
                {
                        $add_dname=$_GET["dname"];
                        bbs_add_favboarddir($add_dname);
                }
                if (isset($_GET["bname"]))
                {
                        $add_bname=$_GET["bname"];
                        $sssss=bbs_add_favboard($add_bname);
                }
                $boards = bbs_fav_boards($select, 1);
                if ($boards == FALSE)
                        html_error_quit("��ȡ���б�ʧ��");
?>
<body>
<table width="100%" border="0" cellspacing="0" cellpadding="3" >
  <tr> 
    <td colspan="2" class="kb2" colspan=2>
	    <a class="kts1" href="mainpage.php"><?php echo BBS_FULL_NAME; ?></a>  - <a class="kts1"  href="bbsfav.php">���˶�����</a></td>
  </tr>
   <tr valign=bottom align=center> 
    <td align="left" class="kb4"><?php if( $select===-1 ) echo "[��Ŀ¼]"; ?></td>
     <td align="right" class="kb1" >
	   <a class="kts1" href="bbssec.php">����������</a>
    </td>
         
  </tr>
   <tr> 
    <td colspan="2" height="9" background="images/dashed.gif"> </td>
  </tr>
   <tr><td colspan="2" align="center">

<table width="100%" border="0" cellspacing="0" cellpadding="3" class="kt1">
<tr>
<td class="kt2" width="2%"> </td>
<td class="kt2" width="2%"> </td>
<td class="kt2" width="23%">����������</td>
<td class="kt2" width="10%">���</td>
<td class="kt2" width="38%">��������</td>
<td class="kt2" width="14%">����</td>
<td class="kt2" width="5%">ƪ��</td>
<td class="kt2" width="6%"> </td>
</tr>
<?php
                $brd_name = $boards["NAME"]; // Ӣ����
                $brd_desc = $boards["DESC"]; // ��������
                $brd_class = $boards["CLASS"]; // �������
                $brd_bm = $boards["BM"]; // ����
                $brd_artcnt = $boards["ARTCNT"]; // ������
                $brd_unread = $boards["UNREAD"]; // δ�����
                $brd_zapped = $boards["ZAPPED"]; // �Ƿ� z ��
                $brd_position= $boards["POSITION"];//λ��
                $brd_flag= $boards["FLAG"];//Ŀ¼��ʶ
                $brd_bid= $boards["BID"];//Ŀ¼��ʶ
                $rows = sizeof($brd_name);
                if($select != -1)
                {
?>
	<tr>
	<td class="kt3 c2" height="25"> </td>
	<td class="kt4 c2"> <img src="images/groupgroup.gif" height="15" width="20" alt="up" title="�ص���һ��"></td>
	<td class="kt3 c3" colspan="6" align="left"><a class="kts1" href="bbsfav.php?select=<?php echo $list_father; ?>">�ص���һ��</a>
	</td>
	</tr>
<?php
                }
                for ($i = 0; $i < $rows; $i++)  
                {
                if( $brd_unread[$i] ==-1 && $brd_artcnt[$i] ==-1)
                        continue;
?>
<tr>
<td class="kt3 c2" align=center height=25><?php echo $i+1; ?></td>
<?php
                        if ($brd_flag[$i] ==-1 )
                        {
?>
        <td class="kt4 c2"> <img src="images/groupgroup.gif" height="15" width="20"  alt="��" title="������"></td><td class="kt3 c3"><a class="kts1" href="bbsfav.php?select=<?php echo $brd_bid[$i];?>&up=<?php echo $select; ?>">
        <?php echo $brd_desc[$i];?>
        </a></td>
        <td class="kt3 c2">[Ŀ¼]</td>
        <td class="kt3 c2" colspan="3">&nbsp;</td>
        <td class="kt3 c2"><a class="kts1" href="bbsfav.php?select=<?php echo $select;?>&delete=<?php echo $brd_bid[$i];?>">ɾ��</a></td>
        </tr>   
<?php
                                continue;
                        }
                        if ($brd_unread[$i] == 1) {
?>
<td class="kt4 c1"> <img src="images/newgroup.gif"  height="15" width="20" alt="��" title="δ����־"></td><td class="kt3 c1">
<?php                              
                        } else {
?>
<td class="kt4 c1" > <img src="images/oldgroup.gif" height="15" width="20"   alt="��" title="�Ѷ���־"></td><td class="kt3 c1">
<?php
                        }
                        if ($brd_zapped[$i] == 1)
                                echo "*";
                        else
                                echo "&nbsp;";
?><a class="kts1" href="/bbsdoc.php?board=<?php echo urlencode($brd_name[$i]); ?>"><?php echo $brd_name[$i]; ?></a>
</td>
<td class="kt3 c3" align="center"><?php echo $brd_class[$i]; ?></td>
<td class="kt3 c1">&nbsp;&nbsp;
<a class="kts1" href="/bbsdoc.php?board=<?php echo urlencode($brd_name[$i]); ?>"><?php echo $brd_desc[$i]; ?></a>
</td>
<td class="kt3 c2" align="center">
<?php
                        $bms = explode(" ", trim($brd_bm[$i]));
                        if (strlen($bms[0]) == 0 || $bms[0][0] <= chr(32))
                                echo "����������";
                        else
                        {
                                if (!ctype_print($bms[0][0]))
                                        echo $bms[0];
                                else
                                {
?>
<a class="kts1" href="/bbsqry.php?userid=<?php echo $bms[0]; ?>"><?php echo $bms[0]; ?></a>
<?php
                                }
                        }
?>
</td>
<td class="kt3 c1"><?php echo $brd_artcnt[$i]; ?></td>
<td class="kt3 c2">
<a class="kts1" href="bbsfav.php?select=<?php echo $select;?>&delete=<?php echo bbs_is_favboard($brd_position[$i])-1;?>">
ɾ��</a>
</td>
</tr>
<?php
                }
?>
</table>
   <tr> 
    <td colspan="2" height="9" background="images/dashed.gif"> </td>
  </tr>
  </table>
<center>
<form action=bbsfav.php>����Ŀ¼<input name=dname size=24 maxlength=20 type=text value=""><input type=submit value=ȷ��><input type=hidden name=select value=<?php echo $select;?>><input type=hidden name=up value=<?php echo $list_father;?>></form>
<form action=bbsfav.php>���Ӱ���<input name=bname size=24 maxlength=20 type=text value=""><input type=submit value=ȷ��><input type=hidden name=select value=<?php echo $select;?>><input type=hidden name=up value=<?php echo $list_father;?>></from>
</center>


<?php
                bbs_release_favboard();
                html_normal_quit();
        }
?>
