package view;

import com.google.gson.Gson;
import dao.BaseDAO;
import model.Maze;
import model.Position;
import util.MazeJsonParser;
import util.ProcessUtil;

import javax.swing.*;
import java.awt.*;
import java.nio.file.Paths;
import java.util.Map;
import java.util.Objects;

public class MazeFrame extends JFrame {
    private final int CELL_SIZE=30;
    private MazePanel mazePanel;
    private ControlPanel controlPanel;
    private Maze currentMaze;
    private long lastMazeId = -1;
    public MazeFrame() {
        super("迷宫生成与求解");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        mazePanel = new MazePanel();
        controlPanel = new ControlPanel();
        add(new JScrollPane(mazePanel), BorderLayout.CENTER);
        add(new JScrollPane(controlPanel), BorderLayout.EAST);
        pack();
        setLocationRelativeTo(null);
    }
    class MazePanel extends JPanel{
        public MazePanel(){
            setPreferredSize(new Dimension(600,600));
            setBackground(Color.GRAY);
        }
        @Override
        protected void paintComponent(Graphics g){
            super.paintComponent(g);
            if(currentMaze == null|| currentMaze.grid==null)
                return;
            for(int i=0;i<currentMaze.rows;i++){
                for(int j=0;j<currentMaze.cols;j++){
                    if (currentMaze.grid[i][j] == 1) {
                        g.setColor(Color.BLACK); // 墙
                    } else {
                        g.setColor(Color.WHITE); // 路
                    }
                    g.fillRect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                    g.setColor(Color.LIGHT_GRAY);
                    g.drawRect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                }
            }
            if (currentMaze.entrance != null) {
                g.setColor(Color.GREEN);
                g.fillRect(currentMaze.entrance.y * CELL_SIZE, currentMaze.entrance.x * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                g.setColor(Color.BLACK);
                g.drawRect(currentMaze.entrance.y * CELL_SIZE, currentMaze.entrance.x * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            }
            if (currentMaze.exit != null) {
                g.setColor(Color.RED);
                g.fillRect(currentMaze.exit.y * CELL_SIZE, currentMaze.exit.x * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                g.setColor(Color.BLACK);
                g.drawRect(currentMaze.exit.y * CELL_SIZE, currentMaze.exit.x * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            }
            if(currentMaze.path!=null && !currentMaze.path.isEmpty()){
                g.setColor(Color.BLUE);
                for(Position pos:currentMaze.path){
                    int x=pos.y*CELL_SIZE+CELL_SIZE/4;
                    int y=pos.x*CELL_SIZE+CELL_SIZE/4;
                    g.fillOval(x,y,CELL_SIZE/2,CELL_SIZE/2);
                }
            }
        }
    }
    public void updateSize() {
        if (currentMaze != null) {
            int width = currentMaze.cols * CELL_SIZE;
            int height = currentMaze.rows * CELL_SIZE;
            mazePanel.setPreferredSize(new Dimension(width, height));
            mazePanel.revalidate();
            SwingUtilities.invokeLater(this::pack);
        }
    }
    class MazeIDItem{
        private final long id;
        private final String displayText;

        public MazeIDItem(long id, String displayText) {
            this.id = id;
            this.displayText = displayText;
        }

        public long getId() {
            return id;
        }

        @Override
        public String toString() {
            return displayText;
        }
    }
    class ControlPanel extends JPanel{
        JTextField rowsField=new JTextField("21",5);
        JTextField colsField=new JTextField("21",5);
        JTextField entranceField=new JTextField("1,1",5);
        JTextField exitField=new JTextField("19,19",5);
        JComboBox<MazeIDItem> mazeIdBox=new JComboBox<>();
        JComboBox<String> genAlgoBox = new JComboBox<>(new String[]{"Kruskal", "Prim", "DFS"});
        JComboBox<String> solAlgoBox = new JComboBox<>(new String[]{"DFS", "BFS", "A*"});
        JComboBox<String> connBox = new JComboBox<>(new String[]{"4联通", "8联通"});
        JButton genButton = new JButton("生成迷宫");
        JButton solButton = new JButton("求解迷宫");
        public ControlPanel(){
            setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
            setBorder(BorderFactory.createEmptyBorder(10,10,10,10));

            JPanel genPanel = new JPanel(new GridLayout(5,2,5,5));
            genPanel.setMaximumSize(new Dimension(250,200));

            genPanel.add(new JLabel("行数:"));
            genPanel.add(rowsField);
            genPanel.add(new JLabel("列数"));
            genPanel.add(colsField);
            genPanel.add(new JLabel("入口(r,c)"));
            genPanel.add(entranceField);
            genPanel.add(new JLabel("出口(r,c)"));
            genPanel.add(exitField);
            genPanel.add(new JLabel("生成算法"));
            genAlgoBox.setMaximumSize(new Dimension(120,30));
            genPanel.add(genAlgoBox);
            add(genPanel);
            add(Box.createVerticalStrut(10));
            genButton.setAlignmentX(Component.CENTER_ALIGNMENT);
            add(genButton);

            add(Box.createVerticalStrut(15));
            add(new JSeparator());
            add(Box.createVerticalStrut(15));

            JPanel solPanel = new JPanel(new GridLayout(3,3,5,5));
            solPanel.setMaximumSize(new Dimension(250,200));
            solPanel.add(new JLabel("迷宫ID"));
            solPanel.add(mazeIdBox);
            solPanel.add(new JLabel("求解算法"));
            solPanel.add(solAlgoBox);
            solPanel.add(new JLabel("联通方式"));
            solPanel.add(connBox);

            add(solPanel);
            add(Box.createVerticalStrut(10));
            solButton.setAlignmentX(Component.CENTER_ALIGNMENT);
            add(solButton);

            add(Box.createVerticalGlue());
            refreshMazeId();
            genButton.addActionListener(e -> generateAction());
            solButton.addActionListener(e -> solveAction());
        }
        private void refreshMazeId(){
            try {
                mazeIdBox.removeAllItems();
                Map<Long,String> infos = BaseDAO.getMazeInfo();
                String[] testDetails = {
                        "出入重合","全空迷宫","死胡同","蛇形长廊","常规迷宫1",
                        "常规迷宫2","联通性对比"
                };
                MazeIDItem targetToSelect = null;
                for (Map.Entry<Long,String> entry: infos.entrySet()) {
                    long id = entry.getKey();
                    String genType = entry.getValue();
                    String detail;
                    if(id>=1 && id<=7){
                        int index = (int)(id-1);
                        String name = testDetails[index];
                        detail = id + "[" + name + "]";
                    }else {
                        detail = id + "["+genType+"生成]";
                    }
                    MazeIDItem item = new MazeIDItem(id, detail);
                    mazeIdBox.addItem(item);
                    if(lastMazeId == id) {
                        targetToSelect = item;
                    }
                }
                if (targetToSelect != null) {
                    mazeIdBox.setSelectedItem(targetToSelect);
                }
            } catch (Exception e) {
                System.err.println("加载迷宫ID列表失败"+e);
            }
        }
        private void generateAction(){
            try{
                String workDir = System.getProperty("user.dir");
                String genExe = Paths.get(workDir, "bin","Generate.exe").toString();
                int r = Integer.parseInt(rowsField.getText());
                int c = Integer.parseInt(colsField.getText());
                String[] entArr = entranceField.getText().split(",");
                String[] exitArr = exitField.getText().split(",");
                String inputConfig = String.format(
                        "{\"rows\":%d, \"cols\":%d, \"entrance\":[%s,%s], \"exit\":[%s,%s], \"generateType\":\"%s\"}",
                        r, c, entArr[0], entArr[1], exitArr[0], exitArr[1], genAlgoBox.getSelectedItem()
                );
                String genJson= ProcessUtil.runAndRead(genExe,inputConfig);
                currentMaze= MazeJsonParser.parse(genJson);
                updateSize();
                lastMazeId = BaseDAO.insertMazeBase(r, c,
                        Integer.parseInt(entArr[0]), Integer.parseInt(entArr[1]),
                        Integer.parseInt(exitArr[0]), Integer.parseInt(exitArr[1]),
                        genJson, Objects.requireNonNull(genAlgoBox.getSelectedItem()).toString());
                refreshMazeId();
                mazePanel.repaint();
                JOptionPane.showMessageDialog(null, "生成成功！MazeID: " + lastMazeId);
            } catch (Exception e) {
                System.err.println(e.getMessage());
                JOptionPane.showMessageDialog(null, "生成失败: " + e.getMessage());
            }
        }
        private void solveAction(){
            try{
                String workDir = System.getProperty("user.dir");
                String solExe = Paths.get(workDir, "bin","Solve.exe").toString();
                MazeIDItem selectedItem = (MazeIDItem) mazeIdBox.getSelectedItem();
                if(selectedItem == null || selectedItem.toString().trim().isEmpty()){
                    JOptionPane.showMessageDialog(null, "请输入有效的迷宫ID");
                    return;
                }
                long inputId=selectedItem.getId();
                if(currentMaze==null||inputId!=lastMazeId){
                    String dbJson= BaseDAO.getMazeJsonById(inputId);
                    if(dbJson==null || dbJson.isEmpty()){
                        throw new RuntimeException("数据库中不存在 ID 为 " + inputId + " 的迷宫数据");
                    }
                    currentMaze= MazeJsonParser.parse(dbJson);
                    lastMazeId=inputId;
                }
                String solAlgo=(String) solAlgoBox.getSelectedItem();

                String solveType = solAlgo.equals("A*")?"AStar":solAlgo;

                int connectivity=connBox.getSelectedIndex()==0?4:8;

                String mazeJson=new Gson().toJson(currentMaze);
                String finalJson=mazeJson.substring(0,mazeJson.length()-1)+", \"solveType\":\"" + solveType + "\", " +"\"connected\":" + connectivity + "}";
                long startTime = System.currentTimeMillis();
                String solJson= ProcessUtil.runAndRead(solExe,finalJson);
                long endTime = System.currentTimeMillis();
                int solveTimeMs = (int) (endTime - startTime);
                Maze solData= MazeJsonParser.parse(solJson);
                updateSize();
                if(solData.path==null){
                    throw new RuntimeException("求解程序未返回有效路径");
                }
                currentMaze.path=solData.path;
                BaseDAO.insertMazeSolve(
                        inputId,
                        Objects.requireNonNull(solAlgoBox.getSelectedItem()).toString(),
                        connectivity,
                        new Gson().toJson(solData.path),
                        solData.path.size(),
                        solveTimeMs);
                mazePanel.repaint();
                if(!solData.path.isEmpty())
                    JOptionPane.showMessageDialog(null, "求解完成，路径长度: " + solData.path.size());
                else
                    JOptionPane.showMessageDialog(null,"当前迷宫无可行路径");
            }catch (NumberFormatException e){
                JOptionPane.showMessageDialog(null,"ID必须为数字");
            }catch (Exception e){
                System.err.println(e.getMessage());
                JOptionPane.showMessageDialog(null, "求解失败: " + e.getMessage());
            }
        }
    }

    public static void main(String[] args) {
        try{
            BaseDAO.getConnection().close();
            System.out.println("数据库连接成功");
        }catch (Exception e){
            e.printStackTrace();
            JOptionPane.showMessageDialog(null,
                    "数据库初始化失败，请检查驱动依赖或环境: \n" + e.getMessage(),
                    "严重错误",
                    JOptionPane.ERROR_MESSAGE);
            return;
        }
        SwingUtilities.invokeLater(()->{
            try{
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            }catch (Exception e){
                e.printStackTrace();
            }
            MazeFrame mazeFrame = new MazeFrame();
            mazeFrame.setVisible(true);
        });
    }
}
