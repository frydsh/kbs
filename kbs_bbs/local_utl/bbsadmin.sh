#!/bin/sh
#
#       define the standout() and standend() escape sequences...
#
#SO="^[[7m"
#SE="^[[0m"
#stty pass8

#biff n
cd /home/bbs
while true
do
clear
echo "---------------------------------------------"
echo "         ����BBSϵͳ����	"
echo "---------------------------------------------"
echo "
   [1] ת��BBS(�����������֮���ñ��˵���������BBS)
   [2] ע����ߺ�ָ�ע�ᵥ
   [3] ֹͣ��½ϵͳ
   [4] �����������Ż��� 
   [Q] �˳�

"
echo  "��ѡ�� [Q]: "

        if read CHOICE
           then
            clear
            case "${CHOICE}"
              in
                '')
                  break
                  ;;
                'Q')
                  break
                  ;;
                'q')
                  break
                  ;;
                1)
		  telnet -E localhost 8002
		  break
                  ;;
                2)
                  echo " ��new_register.tmp����new_register"
		  cat new_register.tmp >> new_register
                  echo "ɾ���ļ�new_register.tmp"
		  rm -f new_register.tmp
                  ;;
		3)
		  echo "����ֹͣ��½��ʾ����Ctrl+D����"
		  cat > /home/bbs/NOLOGIN
		  ;;
		4)
		  /home/bbs/bin/poststat /home/bbs
		  /home/bbs/bin/htmltop10 /home/bbs
		  ;;
                *)
                  echo ""
                  echo "�����ѡ��"

                  ;;
            esac
          else
            exit
        fi
done

clear

