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
                        $list_father=-1;
                settype($list_father,"integer");
                
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
<style type="text/css">A {color: #0000f0}</style>
<body>
<center><?php echo BBS_FULL_NAME; ?> -- ���˶�����
<?php
 if( $select===-1 ) echo "[��Ŀ¼]";
?>
<a href="bbssec.php">����������</a>
<hr class="default"/>
<table width="610">
<tr>
<td>���</td>
<td>δ</td>
<td>����������</td>
<td>���</td>
<td>��������</td>
<td>����</td>
<td>������</td>
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
<td></td>
<td><img src=/images/folder.gif></td>
<td><a href="bbsfav.php?select=<?php echo $list_father; ?>">�ص���һ��</a></td>
<td colspan=4></td>
</tr>
<?php
                }
                for ($i = 0; $i < $rows; $i++)  
                {
                if( $brd_unread[$i] ==-1 && $brd_artcnt[$i] ==-1)
                        continue;
?>
<tr>
<td><?php echo $i+1; ?></td>
<td>
<?php
                        if ($brd_flag[$i] ==-1 )
                        {
?>
        <img src=/images/folder.gif></td>
        <td>
        <a href="bbsfav.php?select=<?php echo $brd_bid[$i];?>&up=<?php echo $select; ?>">
        <?php echo $brd_desc[$i];?>
        </a></td>
        <td colspan=4></td>
        <td><a href="bbsfav.php?select=<?php echo $select;?>&delete=<?php echo $brd_bid[$i];?>">ɾ��</a></td>
        </tr>   
<?php
                                continue;
                        }
                        if ($brd_unread[$i] == 1)
                                echo "��";
                        else
                                echo "��";
?>
</td>
<td>
<?php
                        if ($brd_zapped[$i] == 1)
                                echo "*";
                        else
                                echo "&nbsp;";
?><a href="/bbsdoc.php?board=<?php echo urlencode($brd_name[$i]); ?>"><?php echo $brd_name[$i]; ?></a>
</td>
<td><?php echo $brd_class[$i]; ?></td>
<td>
<a href="/bbsdoc.php?board=<?php echo urlencode($brd_name[$i]); ?>"><?php echo $brd_desc[$i]; ?></a>
</td>
<td>
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
<a href="/bbsqry.php?userid=<?php echo $bms[0]; ?>"><?php echo $bms[0]; ?></a>
<?php
                                }
                        }
?>
</td>
<td><?php echo $brd_artcnt[$i]; ?></td>
<td>
<a href="bbsfav.php?select=<?php echo $select;?>&delete=<?php echo bbs_is_favboard($brd_position[$i])-1;?>">
ɾ��</a>
</td>
</tr>
<?php
                }
?>
</table>
<br>
<form action=bbsfav.php>����Ŀ¼<input name=dname size=24 maxlength=20 type=text value=""><input type=submit value=ȷ��><input type=hidden name=select value=<?php echo $select;?>></form>
<form action=bbsfav.php>���Ӱ���<input name=bname size=24 maxlength=20 type=text value=""><input type=submit value=ȷ��><input type=hidden name=select value=<?php echo $select;?>></from>
<hr class="default"/>
</center>

<?php
                bbs_release_favboard();
                html_normal_quit();
        }
?>
