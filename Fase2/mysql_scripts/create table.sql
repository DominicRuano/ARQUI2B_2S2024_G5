
drop table ace2_p1_g5.device_data;
create table ace2_p1_g5.device_data(
id int AUTO_INCREMENT not null ,
device_src varchar(40),
val decimal(4,2),
date_post TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
primary key(id)
);
#DROP TRIGGER ace2_p1_g5.default_datetime
/*DELIMITER $$ 
CREATE TRIGGER ace2_p1_g5.default_datetime BEFORE INSERT  
    ON ace2_p1_g5.device_data 
      FOR EACH ROW BEGIN  
        SET NEW.date_post = CURRENT_TIMESTAMP;   
END$$ DELIMITER ;*/
select * from ace2_p1_g5.device_data;