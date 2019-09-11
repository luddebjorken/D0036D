/*
    Author: Ludvig Björk Förare
    Description: A class for handling the games logic + communication during runtime
    
    Members:
    int tick;
    int state;
    int delay;

    int[][] pos;
    int[][] field;

    int playerCount;
    int id;
    gamePanel panel;

    public Socket server;
    DatagramSocket in;
    DataOutputStream out;

    void update()
    void move(int x, int y)
 
*/
package trongame;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class gameThread extends Thread{
    
    int tick = 1;
    int state = 1;
    int delay = 250;
    int[][] pos;
    int[][] field;
    int playerCount;
    int id;

    public Socket server;
    DatagramSocket in;
    DataOutputStream out;

    gamePanel panel;
    
    //  Constructor
    //  Socket s
    //  Gp -> game panel reference
    //  Playercount -> Number of players in game
    //  id -> Local player ID
    public gameThread(Socket s, gamePanel gp, int playerCount, int id){
        server = s;
        panel = gp;
        this.playerCount = playerCount;
        this.id = id;
        
        pos = new int[playerCount][2];
        
        //Places players in their locations
        try{
            pos[0] = new int[]{0,0};
            pos[1] = new int[]{31,31};
            pos[2] = new int[]{0,31};
            pos[3] = new int[]{31,0};
        }catch(ArrayIndexOutOfBoundsException e){ //Exits when all allocated array spaces are filled
            
        }
        
        //Sets up iostreams
        field = new int[32][32];
        try {
            out = new DataOutputStream(server.getOutputStream());
            in = new DatagramSocket(25566+id);
        } catch (IOException ex) {
            Logger.getLogger(gamePanel.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        //Fills the field with empty spaces
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                field[i][j] = -1;
            }
        }
        
        //Colors field
        for (int i = 0; i < playerCount; i++) {
            field[pos[i][0]][pos[i][1]] = i;
        }
    }
    
     
    //Update is run once per tick. Receives and handles position updates from server
    public void update(){
        
        try {
            //RECEIVES POSITIONS
            byte[] buf = new byte[256];
            DatagramPacket packet = new DatagramPacket(buf, buf.length);
            in.receive(packet);
            String msg = new String(packet.getData(), 0, packet.getLength());
            
            tick++;
            if(msg.charAt(0)=='1'){
                System.out.println("PosMessage: " + msg);
                int index = msg.indexOf('|');
                int sTick = Integer.parseInt(msg.substring(index+1, msg.indexOf('|', index+1)));
                if(sTick!=tick){
                    System.out.println("Unsynced! " + tick + ":" + sTick);
                }
                index = msg.indexOf('|', index+1);
                int n = Integer.parseInt(msg.substring(index+1, msg.indexOf('|', index+1)));
                index = msg.indexOf('|', index+1);
                
                //Changed positions
                for (int i = 0; i < n; i++) {
                    System.out.println("N; " + i);
                    pos[i] = new int[]{
                        Integer.parseInt(msg.substring(index+1, msg.indexOf(',',index+1))),
                        Integer.parseInt(msg.substring(msg.indexOf(',',index+1)+1, msg.indexOf(';',index+1)))};
                    System.out.println(i + "@Position: " + pos[i][0] + ":" + pos[i][1]);
                    index = msg.indexOf(';', index + 1);
                }
            }else if(msg.charAt(0)=='2'){
                //RESERVED
            }else if(msg.charAt(0)=='3'){
                //A player has won
                panel.winnerName = msg.substring(msg.indexOf('|')+1);
                state = 2;
            }
            
            panel.repaint();
        } catch (IOException ex) {
            Logger.getLogger(gamePanel.class.getName()).log(Level.SEVERE, null, ex);
        }
        if(state==1) update();
        
    }
    
    //Sends move command to server
    public void move(int x, int y){
        try {
            out.writeUTF("2|" + id + "|" + x + "," + y);
            out.flush();
        } catch (IOException ex) {
            Logger.getLogger(gameThread.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
