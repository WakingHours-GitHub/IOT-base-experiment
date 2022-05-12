package base_experiment;

import javax.swing.*;
import java.awt.*;
import java.util.Scanner;

public class main {
    // 初始化SQL对象：
    static SQL_utile sql_utile = null;

    public static void main(String[] args) throws Exception {
        sql_utile = new SQL_utile();
        run();

    }

    public static void run() {

        System.out.println("please input your choose: ");
        Scanner choose = new Scanner(System.in);
        boolean flag = true;
        while (flag) {
            menu();

            switch (choose.next()) {
                case "1" -> { // 添加用户

                    addUser();

                    break;
                }
                case "2" -> { // 查询用户
                    queryAll();
                    break;
                }
                case "3" -> { // 删除用户
                    delete_by_id();

                    break;
                }
                case "4" -> { // 退出
                    flag = false;
                    System.out.println("退出智慧家居管理系统");
                }
                default -> {
                    System.out.println("not choose, please re-input");
                    break;
                }
            }
        }


    }

    private static void addUser() {
        Scanner in = new Scanner(System.in);
        System.out.println("please input user information(split by space, input 'exit' to exit): ");
        while (true) {
//            System.out.println(in.next());
            String next = in.nextLine();
            if (next.equals("exit")) {
                break;
            }
            String[] s = next.split(" "); // input data, and process -> split by space

            sql_utile.add(new User(s)); // 添加用户
        }
        System.out.println(); // 打印回车
        System.out.println("add success");


    }


    private static void queryAll() { // 查询全部数据
        System.out.println("all user follow: ");
        sql_utile.query();

    }

    private static void delete_by_id() {
        System.out.print("请输入需要删除用户的id：");
        int id = new Scanner(System.in).nextInt();

        sql_utile.delete_by_id(id);
        System.out.println("delete success");
    }

    public static void menu() {
        System.out.println("************************************");
        System.out.println("********** 智慧家居管理系统 **********");
        System.out.println("********** 1. 添加用户 **************");
        System.out.println("********** 2. 查询全部 **************");
        System.out.println("********** 3. 删除用户 **************");
        System.out.println("********** 4. 退出     **************");
        System.out.println("************************************");
    }

    public static void window() {
        JFrame window = new JFrame("智能家具数据库"); // 创建一个窗口
        Container pane = window.getContentPane();
        window.setBounds(50, 50, 500, 600);

        JTextField id = new JTextField();
        JTextField name = new JTextField();
        JTextField gender = new JTextField();
        JTextField temperature = new JTextField();
        JTextField humidness = new JTextField();
        JLabel id_lable = new JLabel("id: ");
        JLabel name_lable = new JLabel("name: ");
        JLabel gender_lable = new JLabel("gender: ");
        JLabel temp_lable = new JLabel("temperature: ");
        JLabel hum_lable = new JLabel("humidness: ");

        JButton add = new JButton("添加");
        JButton clear = new JButton("全部清楚");


        pane.setLayout(new FlowLayout());

        pane.add(id_lable);
        pane.add(BorderLayout.CENTER, id);
        pane.add(name_lable);
        pane.add(BorderLayout.CENTER, name);


        window.setVisible(true);

    }
}
