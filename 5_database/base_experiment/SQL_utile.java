package base_experiment;


import java.sql.*;

public class SQL_utile {
    String jdbc = "jdbc:mysql://127.0.0.1:3306/housing";
    Connection connection = null;
    Statement state = null;   //容器, 获取执行sql的对象 -> statement
    private String userName = "root";
    private String password = "root";


    public SQL_utile() { //

    }


    public void add(User user) {
        try {

            /*
            注意, 使用MySql 5一下的版本, 使用老版本驱动,  使用MySql 6以上, 使用新版本驱动.
             */
            Class.forName("com.mysql.jdbc.Driver"); // 加载驱动
//            Class.forName("com.mysql.cj.jdbc.Driver"); // 新版本
            connection = DriverManager.getConnection(jdbc, userName, password); // 登录

            state = connection.createStatement(); // 生成容器.
            String sql = null;
            // 生成SQL语句, 进行执行
            String name = user.getName();
            if (user.getId() == -1) {
                sql = "INSERT INTO test " +
                        "VALUES(null, " + "'" + user.getName() + "'" + "," + "'" + user.getGender() + "'" + "," + user.getTemperature() + "," + user.getHumidness() + ")";   //SQL语句
            } else {
                sql = "INSERT INTO test " +
                        "VALUES(" + user.getId() + "," + "'" + user.getName() + "'" + "," + "'" + user.getGender() + "'" + "," + user.getTemperature() + "," + user.getHumidness() + ")";
            }
            System.out.println(sql);
//            int i = state.executeUpdate(sql);//将sql语句上传至数据库执行
            state.execute(sql);

//            System.out.println(i); // 打印状态
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (SQLException e) {
            e.printStackTrace();
        } finally {
            // 释放资源
//            state.close();
//            connection.close();
            //stmt.close();
            //7. 释放资源
            //避免空指针异常
            if (state != null) {
                try {
                    state.close();
                } catch (SQLException e) {
                    e.printStackTrace();
                }
            }

            if (connection != null) {
                try {
                    connection.close();
                } catch (SQLException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void query() {

        try {
            Class.forName("com.mysql.jdbc.Driver"); // 加载驱动
// 获取内容
            connection = DriverManager.getConnection(jdbc, userName, password); // 登录
            state = connection.createStatement(); // 生成容器
// 生成SQL语句,
            String sql = "select * from test"; // 查询
            ResultSet resultSet = state.executeQuery(sql);
            while (resultSet.next()) {
                int id = resultSet.getInt("id");
                String name = resultSet.getString("name");
                String gender = resultSet.getString("gender");
                Float temperature = resultSet.getFloat("temperature");
                Float humidness = resultSet.getFloat("humidness");
                System.out.println("user: id:" + id + ", name:" + name + ", gender:" + gender + ", temperature:" + temperature + ", humidness:" + humidness);


            }
            resultSet.close(); // close resource


        } catch (SQLException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } finally {
            if (state != null) {
                try {
                    state.close();
                } catch (SQLException e) {
                    e.printStackTrace();
                }
            }


            if (connection != null) {
                try {
                    connection.close();
                } catch (SQLException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void delete_by_id(int id) {
        try {
            Class.forName("com.mysql.jdbc.Driver"); // 加载驱动
            // 获取内容
            connection = DriverManager.getConnection(jdbc, userName, password); // 登录
            state = connection.createStatement(); // 生成容器
            // 生成SQL语句,
            String sql = "delete from test where id=" + id; // 查询
            boolean execute = state.execute(sql);


        } catch (SQLException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } finally {
            if (state != null) {
                try {
                    state.close();
                } catch (SQLException e) {
                    e.printStackTrace();
                }
            }


            if (connection != null) {
                try {
                    connection.close();
                } catch (SQLException e) {
                    e.printStackTrace();
                }
            }
        }

    }


}
