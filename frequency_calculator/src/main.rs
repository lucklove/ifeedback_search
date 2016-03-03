extern crate mysql;
extern crate redis;
extern crate core;

use mysql as my;
use std::fmt::Write;
use redis::Commands;


fn main() {
    let pool = my::Pool::new("mysql://root:lucklove@127.0.0.1").unwrap();

    let keys : Vec<String> = pool.prep_exec("select distinct keyword from db_ifeedback_search.tb_question_keyword", ())
        .map(|result| {
            result.map(|x| x.unwrap()).map(|row| {
                my::from_row::<String>(row)
            }).collect()
        }).unwrap();

    let subjects : Vec<i32> = pool.prep_exec("select distinct subject from db_ifeedback_search.tb_question_keyword", ())
        .map(|result| {
            result.map(|x| x.unwrap()).map(|row| {
                my::from_row::<i32>(row)
            }).collect()
        }).unwrap();

    let types : Vec<i32> = pool.prep_exec("select distinct type from db_ifeedback_search.tb_question_keyword", ())
        .map(|result| {
            result.map(|x| x.unwrap()).map(|row| {
                my::from_row::<i32>(row)
            }).collect()
        }).unwrap();

    let client = redis::Client::open("redis://127.0.0.1/").unwrap();
    let con = client.get_connection().unwrap();

    for key in &keys {
        let _ : Vec<()> = pool.prep_exec(r"select (select count(id) from db_ifeedback_search.tb_question_keyword 
            where keyword=?) / (select count(id) from db_ifeedback_search.tb_question_keyword) as freq", (&key,))
            .map(|result| {
                result.map(|x| x.unwrap()).map(|row| {
                    if let Ok(v) = my::from_row_opt::<f32>(row) {
                        let mut s = String::new();
                        let _ = write!(s, "{}#0#0", &key);
                        let _ : () = con.set(s, v).unwrap();
                        println!("{}#0#0 {}", &key, v);
                    }
                }).collect()
            }).unwrap();
         
        for subject in &subjects {
            if *subject == 0 { continue; }
            let _ : Vec<()> = pool.prep_exec(r"select (select count(id) from db_ifeedback_search.tb_question_keyword 
                where keyword=? and subject=?) / (select count(id) from db_ifeedback_search.tb_question_keyword 
                where subject=?)", (&key, &subject, &subject))
                .map(|result| {
                    result.map(|x| x.unwrap()).map(|row| {
                        if let Ok(v) = my::from_row_opt::<f32>(row) {
                            let mut s = String::new();
                            let _ = write!(s, "{}#{}#0", &key, &subject);
                            let _ : () = con.set(s, v).unwrap();
                            println!("{}#{}#0 {}", &key, &subject, v);
                        }
                    }).collect()
                }).unwrap();
        }

        for t in &types {
            if *t == 0 { continue; }
            let _ : Vec<()> = pool.prep_exec(r"select (select count(id) from db_ifeedback_search.tb_question_keyword 
                where keyword=? and type=?) / (select count(id) from db_ifeedback_search.tb_question_keyword 
                where type=?)", (&key, &t, &t))
                .map(|result| {
                    result.map(|x| x.unwrap()).map(|row| {
                        if let Ok(v) = my::from_row_opt::<f32>(row) {
                            let mut s = String::new();
                            let _ = write!(s, "{}#0#{}", &key, &t);
                            let _ : () = con.set(s, v).unwrap();
                            println!("{}#0#{} {}", &key, &t, v);
                        }
                    }).collect()
                }).unwrap();
        }

        for subject in &subjects {
            if *subject == 0 { continue; }
            for t in &types {
                if *t == 0 { continue; }
                let _ : Vec<()> = pool.prep_exec(r"select (select count(id) from db_ifeedback_search.tb_question_keyword
                    where keyword=? and subject = ? and type=?) / (select count(id) from db_ifeedback_search.tb_question_keyword
                    where subject=? and type=?)", (&key, &subject, &t, &subject, &t))
                    .map(|result| {
                        result.map(|x| x.unwrap()).map(|row| {
                            if let Ok(v) = my::from_row_opt::<f32>(row) {
                                let mut s = String::new();
                                let _ = write!(s, "{}#{}#{}", &key, &subject, &t);
                                let _ : () = con.set(s, v).unwrap();
                                println!("{}#{}#{} {}", &key, &subject, &t, v);
                            }
                        }).collect()
                    }).unwrap();
            }
        }
    }
}
